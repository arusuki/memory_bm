// See https://aka.ms/new-console-template for more information
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

public class Program {

    const int MAX_PAYLOAD_SIZE = 50;
    const int INITIAL_NODE_COUNT = 10000;
    const long MUTATION_COUNT = 1000000L;
    const int MAX_MUTATION_SIZE = 200;

    static ulong Next(ulong x) {
        x ^= x >> 30;
        x *= 0Xbf58476d1ce4e5b9UL;
        x ^= x >> 27;
        x *= 0X94d049bb133111ebUL;
        x ^= x >> 31;
        return x;
    }

    static double NextDouble(ulong x) {
        return (double)Next(x) / ulong.MaxValue;
    }

    class Node: IDisposable {

        public long Id { get; }
        public int Size { get; }
        public byte[] Payload { get; }
        public Node Previous { get; set; }
        public Node Next { get; set; }

        public Node(long id) {
            Id = id;
            Size = (int)(NextDouble((ulong)id) * MAX_PAYLOAD_SIZE);
            Payload = new byte[Size];
            for (int i = 0; i < Size; i++) {
                Payload[i] = (byte)i;
            }
        }

        public static void Circle2(Node a, Node b) {
            a.Previous = b;
            a.Next = b;
            b.Previous = a;
            b.Next = a;
        }

        public void Insert(Node n) {
            n.Next = this.Next;
            n.Previous = this;
            this.Next.Previous = n;
            this.Next = n;
        }

        public void Dispose() {
            this.Previous.Next = this.Next;
            this.Next.Previous = this.Previous;
        }
    }

    static void Main0() {
        var ttt0 = Stopwatch.GetTimestamp();

        long nodeId = 0;
        ulong randomState = 0;
        Node head = new Node(nodeId++);
        Node.Circle2(head, new Node(nodeId++));
        for (int i = 2; i < INITIAL_NODE_COUNT; i++) {
            head.Insert(new Node(nodeId++));
        }

        long nodeCount = INITIAL_NODE_COUNT;
        for (long i = 0; i < MUTATION_COUNT; i++) {
            int deleteCount = (int)(NextDouble(randomState++) * MAX_MUTATION_SIZE);
            deleteCount = Math.Min(deleteCount, (int)(nodeCount - 2));
            for (int j = 0; j < deleteCount; j++) {
                Node toDelete = head;
                head = head.Previous;
                toDelete.Dispose();
            }
            nodeCount -= deleteCount;
            int insertCount = (int)(NextDouble(randomState++) * MAX_MUTATION_SIZE);
            for (int j = 0; j < insertCount; j++) {
                head.Insert(new Node(nodeId++));
                head = head.Next;
            }
            nodeCount += insertCount;
        }

        long checksum = 0;
        Node n = head;
        do {
            checksum += n.Id + n.Size;
            if (n.Size > 0) {
                checksum += n.Payload[0];
                checksum += n.Payload[n.Size - 1];
            }
            n = n.Next;
        } while (n != head);

        GC.Collect();
        GC.WaitForPendingFinalizers();

        var sss0 = Stopwatch.GetElapsedTime(ttt0);
        Console.WriteLine($"Elapsed time: {sss0.TotalMilliseconds} ms");
        Console.WriteLine($"Node count: {nodeCount}");
        Console.WriteLine($"Checksum: {checksum:x16}");
    }

    public static void Main() {
        
        for (int i = 0; 10 > i; ++i) {
            Main0();
            Console.WriteLine();
        }
    }
}
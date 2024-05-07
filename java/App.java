public class App {

    private static final int MAX_PAYLOAD_SIZE = 50;
    private static final int INITIAL_NODE_COUNT = 10000;
    private static final long MUTATION_COUNT = 1000000L;
    private static final int MAX_MUTATION_SIZE = 200;

    private static long next(long x) {
        x ^= x >>> 30;
        x *= 0xbf58476d1ce4e5b9L;
        x ^= x >>> 27;
        x *= 0x94d049bb133111ebL;
        x ^= x >>> 31;
        return x;
    }

    private static final double ULONG_MAX_VALUE_AS_DOULBE = 1.8446744073709552E+19;

    private static double nextDouble(long x) {
        double r = (double) next(x) / ULONG_MAX_VALUE_AS_DOULBE;
        return r < 0 ? 1.0 + r : r;
    }

    private static class Node {
        long id;
        int size;
        byte[] payload;
        Node previous;
        Node next;

        Node(long id) {
            this.id = id;
            int s = (int) (nextDouble(id) * MAX_PAYLOAD_SIZE);
            this.size = s;
            this.payload = new byte[s];
            for (int i = 0; i < s; i++) {
                this.payload[i] = (byte) i;
            }
        }

        static void circle2(Node a, Node b) {
            a.previous = b;
            a.next = b;
            b.previous = a;
            b.next = a;
        }

        void insert(Node n) {
            n.next = this.next;
            n.previous = this;
            this.next.previous = n;
            this.next = n;
        }

        void remove() {
            previous.next = next;
            next.previous = previous;
        }
    }

    private static void main0() {
        long startTime = System.nanoTime();

        long nodeId = 0;
        long randomState = 0;
        Node head = new Node(nodeId++);
        Node.circle2(head, new Node(nodeId++));
        for (int i = 2; i < INITIAL_NODE_COUNT; i++) {
            head.insert(new Node(nodeId++));
        }
        long nodeCount = INITIAL_NODE_COUNT;
        for (long i = 0; i < MUTATION_COUNT; i++) {
            int deleteCount = (int) (nextDouble(randomState++) * MAX_MUTATION_SIZE);
            deleteCount = Math.min(deleteCount, (int) (nodeCount - 2));
            for (int j = 0; j < deleteCount; j++) {
                Node toDelete = head;
                head = head.previous;
                toDelete.remove();
            }
            nodeCount -= deleteCount;
            int insertCount = (int) (nextDouble(randomState++) * MAX_MUTATION_SIZE);
            for (int j = 0; j < insertCount; j++) {
                head.insert(new Node(nodeId++));
                head = head.next;
            }
            nodeCount += insertCount;
        }
        long checksum = 0;
        Node n = head;
        do {
            checksum += n.id + n.size;
            if (n.size > 0) {
                checksum += n.payload[0];
                checksum += n.payload[n.size - 1];
            }
            n = n.next;
        } while (n != head);

        System.gc();
        long endTime = System.nanoTime();
        double elapsedTime = (endTime - startTime) / 1e6;
        System.out.println("Elapsed time: " + elapsedTime + " ms");
        System.out.println("Node count: " + nodeCount);
        System.out.printf("Checksum: %016x%n", checksum);
    }


    public static void main(String[] args) {
        for (int i = 0; i < 10; i++) {
            main0();
            System.out.println();
        }
    }
}
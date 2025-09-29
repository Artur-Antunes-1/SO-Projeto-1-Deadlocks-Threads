import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.Semaphore;

public class ProdutorConsumidorSemaforo {

    static final int BUFFER_SIZE = 5;
    static final Queue<Integer> buffer = new LinkedList<>();

    static Semaphore mutex = new Semaphore(1);
    static Semaphore empty = new Semaphore(BUFFER_SIZE);
    static Semaphore full = new Semaphore(0);

    static class Produtor implements Runnable {
        private final int id;

        public Produtor(int id) {
            this.id = id;
        }

        @Override
        public void run() {
            try {
                int item;
                while (true) {
                    item = (int) (Math.random() * 100);
                    Thread.sleep((long) (Math.random() * 3000 + 1000));

                    empty.acquire(); // Espera por um espaço vazio
                    mutex.acquire(); // Entra na região crítica

                    buffer.add(item);
                    System.out.printf("Produtor %d produziu %d. Itens no buffer: %d\n", id, item, buffer.size());

                    mutex.release(); // Sai da região crítica
                    full.release();  // Sinaliza que um espaço foi preenchido
                }
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }

    static class Consumidor implements Runnable {
        private final int id;

        public Consumidor(int id) {
            this.id = id;
        }

        @Override
        public void run() {
            try {
                while (true) {
                    full.acquire();  // Espera por um item
                    mutex.acquire(); // Entra na região crítica

                    int item = buffer.poll();
                    System.out.printf("Consumidor %d consumiu %d. Itens no buffer: %d\n", id, item, buffer.size());

                    mutex.release(); // Sai da região crítica
                    empty.release(); // Sinaliza que um espaço ficou vazio

                    Thread.sleep((long) (Math.random() * 4000 + 1000));
                }
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
            }
        }
    }

    public static void main(String[] args) {
        for (int i = 1; i <= 2; i++) {
            new Thread(new Produtor(i)).start();
        }
        for (int i = 1; i <= 3; i++) {
            new Thread(new Consumidor(i)).start();
        }
    }
}
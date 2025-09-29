import java.util.LinkedList;
import java.util.Queue;

public class ProdutorConsumidorMonitor {

    static final int BUFFER_SIZE = 5;
    static final Queue<Integer> buffer = new LinkedList<>();

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
                    
                    synchronized (buffer) {
                        while (buffer.size() == BUFFER_SIZE) {
                            System.out.printf("Produtor %d encontrou buffer CHEIO. Esperando...\n", id);
                            buffer.wait(); // Espera se o buffer estiver cheio
                        }
                        
                        buffer.add(item);
                        System.out.printf("Produtor %d produziu %d. Itens no buffer: %d\n", id, item, buffer.size());
                        buffer.notifyAll(); // Notifica os consumidores
                    }
                    
                    Thread.sleep((long) (Math.random() * 3000 + 1000));
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
                    synchronized (buffer) {
                        while (buffer.isEmpty()) {
                            System.out.printf("Consumidor %d encontrou buffer VAZIO. Esperando...\n", id);
                            buffer.wait(); // Espera se o buffer estiver vazio
                        }
                        
                        int item = buffer.poll();
                        System.out.printf("Consumidor %d consumiu %d. Itens no buffer: %d\n", id, item, buffer.size());
                        buffer.notifyAll(); // Notifica os produtores
                    }
                    
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
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

public class LeitoresEscritoresRwLock {

    static int sharedData = 0;
    // Criando o lock com política "fair" para dar chance aos escritores
    static final ReadWriteLock rwLock = new ReentrantReadWriteLock(true);
    static final Lock readLock = rwLock.readLock();
    static final Lock writeLock = rwLock.writeLock();

    static class Escritor implements Runnable {
        private final int id;
        public Escritor(int id) { this.id = id; }
        
        @Override
        public void run() {
            try {
                while(true) {
                    writeLock.lock();
                    try {
                        sharedData++;
                        System.out.printf("Escritor %d escreveu: %d\n", id, sharedData);
                    } finally {
                        writeLock.unlock();
                    }
                    Thread.sleep((long) (Math.random() * 5000 + 2000));
                }
            } catch (InterruptedException e) { Thread.currentThread().interrupt(); }
        }
    }

    static class Leitor implements Runnable {
        private final int id;
        public Leitor(int id) { this.id = id; }
        
        @Override
        public void run() {
            try {
                while(true) {
                    readLock.lock();
                    try {
                        System.out.printf("Leitor %d leu: %d\n", id, sharedData);
                    } finally {
                        readLock.unlock();
                    }
                    Thread.sleep((long) (Math.random() * 3000 + 1000));
                }
            } catch (InterruptedException e) { Thread.currentThread().interrupt(); }
        }
    }

    public static void main(String[] args) {
        for (int i = 1; i <= 2; i++) new Thread(new Escritor(i)).start();
        for (int i = 1; i <= 5; i++) new Thread(new Leitor(i)).start();
    }
}
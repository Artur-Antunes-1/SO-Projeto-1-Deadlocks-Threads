import java.util.concurrent.Semaphore;

public class LeitoresEscritoresSemaforo {

    static int sharedData = 0;
    static int readCount = 0;
    static int writeCount = 0;

    static Semaphore mutexReadCount = new Semaphore(1);
    static Semaphore mutexWriteCount = new Semaphore(1);
    static Semaphore resourceAccess = new Semaphore(1);
    static Semaphore readTry = new Semaphore(1);

    static class Escritor implements Runnable {
        private final int id;
        public Escritor(int id) { this.id = id; }
        
        @Override
        public void run() {
            try {
                while(true) {
                    mutexWriteCount.acquire();
                    writeCount++;
                    if (writeCount == 1) readTry.acquire();
                    mutexWriteCount.release();

                    resourceAccess.acquire();
                    sharedData++;
                    System.out.printf("Escritor %d escreveu: %d\n", id, sharedData);
                    resourceAccess.release();

                    mutexWriteCount.acquire();
                    writeCount--;
                    if (writeCount == 0) readTry.release();
                    mutexWriteCount.release();

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
                    readTry.acquire();
                    mutexReadCount.acquire();
                    readCount++;
                    if (readCount == 1) resourceAccess.acquire();
                    mutexReadCount.release();
                    readTry.release();

                    System.out.printf("Leitor %d leu: %d (leitores ativos: %d)\n", id, sharedData, readCount);

                    mutexReadCount.acquire();
                    readCount--;
                    if (readCount == 0) resourceAccess.release();
                    mutexReadCount.release();

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
import java.net.*;
import java.io.*;

public class DateServer {

    public static void main(String[] args) {
        try {
            ServerSocket sock = new ServerSocket(6013);
            while (true) {
                Socket client = sock.accept();
                // Start a new thread to handle the client request
                Thread t = new Thread(new ClientHandler(client));
                t.start();
            }
        } catch (IOException ioe) {
            System.err.println(ioe);
        }
    }
}

class ClientHandler implements Runnable {
    private Socket client;

    public ClientHandler(Socket socket) {
        this.client = socket;
    }

    @Override
    public void run() {
        try {
            PrintWriter pout = new PrintWriter(client.getOutputStream(), true);
            pout.println(new java.util.Date().toString());
            client.close();
        } catch (IOException ioe) {
            System.err.println(ioe);
        }
    }
}

/*
 * server.java is the server for client
 * base code is from: https://www.infoworld.com/article/2853780/socket-programming-for-scalable-systems.html
 * Created by: David Yoo (gy24)
 * Date: Apr 18, 2022
 */

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;
import java.time.format.DateTimeFormatter;  
import java.time.LocalDateTime;  


public class server extends Thread{
    private ServerSocket serverSocket;
    private int port;
    private boolean running = false;

    public server( int port )
    {
        this.port = port;
    }

    public void startServer()
    {
        try
        {
            serverSocket = new ServerSocket( port );
            this.start();
        }
        catch (IOException e)
        {
            e.printStackTrace();
        }
    }

    public void stopServer()
    {
        running = false;
        this.interrupt();
    }

    @Override
    public void run()
    {
        running = true;
        while( running )
        {
            try
            {
                System.out.println( "Listening for a connection" );

                // Call accept() to receive the next connection
                Socket socket = serverSocket.accept();

                // Pass the socket to the RequestHandler thread for processing
                RequestHandler requestHandler = new RequestHandler( socket );
                requestHandler.start();
            }
            catch (IOException e)
            {
                e.printStackTrace();
            }
        }
    }


    public static void main( String[] args )
    {
        if( args.length == 0 )
        {
            System.out.println( "Usage: SimpleSocketServer <port>" );
            System.exit( 0 );
        }
        int port = Integer.parseInt( args[ 0 ] );
        System.out.println( "Start server on port: " + port );

        server server = new server( port );
        server.startServer();

        // Automatically shutdown in 1 minute
        try
        {
            Thread.sleep( 60000 );
        }
        catch( Exception e )
        {
            e.printStackTrace();
        }

        server.stopServer();
    }
}

class RequestHandler extends Thread
{
    private Socket socket;
    RequestHandler( Socket socket )
    {
        this.socket = socket;
    }

    @Override
    public void run()
    {
        try
        {
            // DateTimeFormatter code from: https://beginnersbook.com/2017/11/java-datetimeformatter/
            DateTimeFormatter d = DateTimeFormatter.ofPattern("MM/dd/yyyy HH:mm:ss");  
            // LocalDateTime code from: https://howtodoinjava.com/java/date-time/java-localdatetime-class/
            LocalDateTime now = LocalDateTime.now();  
            // getRemoteSocketAddress() from: https://www.tabnine.com/code/java/methods/java.net.Socket/getRemoteSocketAddress
            System.out.println(d.format(now) + " Received a connection from " + socket.getRemoteSocketAddress().toString());

            // Get input and output streams
            BufferedReader in = new BufferedReader( new InputStreamReader( socket.getInputStream() ) );
            PrintWriter out = new PrintWriter( socket.getOutputStream() );

            // Echo lines back to the client until the client closes the connection or we receive an empty line
            String line = in.readLine();
            // reference: https://www.baeldung.com/java-caesar-cipher
            try{
                int r = Integer.parseInt(line);
                // if the rotation number is valid
                if(r > 0 && r < 26) {
                    out.println(r);
                    out.flush();
                    line = in.readLine();
                    while( line != null && line.length() > 0 ){ 
                        //make an empty string that keeps all the encryted alphabet
                        String res = "";
                        int originalAlphabetPosition;
                        int newAlphabetPosition;
                        char newAlphabet;
                        for(int i = 0; i < line.length(); i++){
                            if (Character.isUpperCase((line.charAt(i)))){
                                originalAlphabetPosition = line.charAt(i) - 'A';
                                newAlphabetPosition = (originalAlphabetPosition + r) % 26;
                                newAlphabet = (char) ('A' + newAlphabetPosition);
                                res += newAlphabet;
                            }
                            else if (line.charAt(i) == ' '){
                                res += ' ';
                            }
                            else{
                                originalAlphabetPosition = line.charAt(i) -'a';
                                newAlphabetPosition = (originalAlphabetPosition + r) % 26;
                                newAlphabet = (char)('a' + newAlphabetPosition);
                                res += newAlphabet;
                            }
                        }
                        out.println(res);
                        out.flush();
                        line = in.readLine();
                    }
                }
                else {
                    out.println("Invalid input");
                    System.exit(0);
                }

                // Close our connection
                in.close();
                out.close();
                socket.close();
                
                System.out.println("Connection closed");

            }
            // NumberFormatException: https://www.geeksforgeeks.org/numberformatexception-in-java-with-examples/
            catch(NumberFormatException e){
                System.out.println("Input is not a number, connection closed");
                System.exit(0);
            }
        }
                 
        catch( Exception e )
        {
            e.printStackTrace();
        }
    }
}
  

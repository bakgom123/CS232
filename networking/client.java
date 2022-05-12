/*
 * client.java is the client that calls the server
 * Created by: David Yoo (gy24)
 * Date: Apr 18, 2022
 */

import java.io.IOException;
import java.net.UnknownHostException;
import java.net.Socket;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintStream;



public class client{

    public static void main(String[] args){
        // welcome message and ask for the rotation amount from the user
        System.out.println("Welcome to Caesar Client!");
        System.out.println("Please enter the rotation number (choose from 1 to 25): ");
        

        // Socket code from: https://docs.oracle.com/javase/tutorial/networking/sockets/readingWriting.html
        String hostName = args[0];
        int portNumber = Integer.parseInt(args[1]);
        try {
            // connect to the server
            Socket s = new Socket(hostName, portNumber);        
            PrintStream out = new PrintStream(s.getOutputStream());
            BufferedReader in =                                           
                new BufferedReader(
                    new InputStreamReader(s.getInputStream()));
            BufferedReader userinput =                                           
                new BufferedReader(
                    new InputStreamReader(System.in));
            
            // read the rotation amount from the user
            String input;
            String output;
            input = userinput.readLine();
            // string to int: https://www.javatpoint.com/java-string-to-int
            if (Integer.parseInt(input) < 0 || Integer.parseInt(input) > 25){
                System.out.println("Invalid input, terminated.");
                System.exit(0);

            }
            out.println(input);
            output = in.readLine();
            

            // if the input is quit, then terminate the program; otherwise, encrypt the text
            while(true){
                if(input.equals("quit")){
                    System.out.println("Terminated");
                    break;
                }
                else if(!output.equals(null)){
                    System.out.println(output);
                    System.out.println("Please enter the text to be encrypted: ");
                    input = userinput.readLine();
                    out.println(input);
                    output = in.readLine();
                }
            }

            in.close();
            out.close();
            s.close();
        }

        catch (UnknownHostException u){
            System.out.println("Invalid address " + hostName);
        }
        catch (IOException e){
            System.err.println("Invalid port " + args[1]);
            System.exit(0);
        }
        // NumberFormatException: https://www.geeksforgeeks.org/numberformatexception-in-java-with-examples/
        catch(NumberFormatException e){
            System.out.println("Input is not a number, program terminated");
            System.exit(0);
        }

    }

}



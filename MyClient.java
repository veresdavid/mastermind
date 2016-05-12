import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

public class MyClient{
	
	// alapértelmezett szerver cím
	public static final String DEFAULT_ADDRESS = "127.0.0.1";
	
	// alapértelmezett szerver port
	public static final int DEFAULT_PORT = 6969;
	
	// színek tömbje
	public static final char[] COLORS = { 'P', 'K', 'Z', 'L', 'S', 'N' };
	
	// függvény, ami ellenőrzi, hogy egy tipp helyesen lett-e megadva
	public static boolean isCorrectGuess(String guess){
		
		// a "feladom" egy speciális eset
		if(guess.equals("feladom")){
			return true;
		}
		
		// jó-e a tipp hossza?
		if(guess.length()!=4){
			System.out.println("A tippednek 4 karakter hosszunak kell lennie!");
			return false;
		}
		
		// jó karakterek alkotják-e a tippet?
		for(int i=0; i<4; i++){
			
			boolean isCorrectCharacter = false;
			
			for(int j=0; j<6; j++){
				if(guess.charAt(i)==COLORS[j]){
					isCorrectCharacter = true;
					break;
				}
			}
			
			if(!isCorrectCharacter){
				System.out.println("Csak a felsorolt szinek szerepelhetnek a tippedben!");
				return false;
			}
			
		}
		
		// ha eddig nem volt hiba, akkor érvényes a tipp
		return true;
		
	}
	
	// függvény, ami eldönti hogy helyes választást gépelt-e be a játékos
	public static boolean isCorrectChoice(String choice){
		
		// csak az "igen" és a "nem" sztringek az elfogadottak
		return ( choice.equals("igen") || choice.equals("nem") );
		
	}
	
	public static void main(String[] args) throws Exception{
		
		// a szerver adatai
		String serverAddress;
		int serverPort;
		
		// ha a felhasználó megadja hogy hova szeretne csatlakozni
		if(args.length==2){
			serverAddress = args[0];
			try{
				serverPort = Integer.parseInt(args[1]);
			}catch(NumberFormatException e){
				// e.printStackTrace();
				System.err.println(" -- HIBA! Nem szamot adtal meg portszamnak! Az alapertelmezettet (6969) hasznaljuk!");
				serverPort = DEFAULT_PORT;
			}
		}else{
			serverAddress = DEFAULT_ADDRESS;
			serverPort = DEFAULT_PORT;
		}
		
		// kliens socket létrehozása
		Socket socket = new Socket(serverAddress, serverPort);
		
		// olvasó és író objektumok létrehozása
		// olvasó a standard bemenetről
		BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in));
		// socket olvasó
		BufferedReader socketIn = new BufferedReader(new InputStreamReader(socket.getInputStream()));
		// socket író
		PrintWriter socketOut = new PrintWriter(socket.getOutputStream(), true);
		
		// a játékos üzenetét tároló sztring
		String myMsg = "";
		
		// a szerver üzenetét tároló sztring
		String serverMsg = "";
		
		System.out.println("Sikeres csatlakozas a szerverhez!");
		
		// színek kiíratása
		System.out.print("Szinek: ");
		for(int i=0; i<6; i++){
			System.out.print(COLORS[i] + " ");
		}
		System.out.println();
		
		// játék
		while(true){
			
			// üzenet olvasása a szervertől
			serverMsg = socketIn.readLine();
			
			// az üzenetnek megfelelő cselekvés végrehajtása
			if(serverMsg.equals("GAME_START")){
				
				System.out.println("Indul a jatek!!!");
				
			}else if(serverMsg.equals("ENTER_GUESS")){
				
				// amíg nem érvényes tipped ad meg a játékos, addig olvassuk a tippjeit
				do{
					System.out.print("Add meg a tipped: ");
				}while(!isCorrectGuess((myMsg = stdIn.readLine())));
				
				// az érvényes tippet elküldjük a szervernek
				socketOut.println(myMsg);
				
			}else if(serverMsg.equals("ROUND_STATS")){
				
				// fejléc beolvasása, kírása
				serverMsg = socketIn.readLine();
				System.out.println(serverMsg);
				
				// 1. játékos adatai, kírása
				serverMsg = socketIn.readLine();
				System.out.println(serverMsg);
				
				// 2. játékos adatai, kírása
				serverMsg = socketIn.readLine();
				System.out.println(serverMsg);
				
			}else if(serverMsg.equals("MATCH_END")){
				
				// fejléc beolvasása, kiírása
				serverMsg = socketIn.readLine();
				System.out.println(serverMsg);
				
				// megfejtés beolvasása, kiírása
				serverMsg = socketIn.readLine();
				System.out.println(serverMsg);
				
				// eredmény beolvasása, kiírása
				serverMsg = socketIn.readLine();
				System.out.println(serverMsg);
				
			}else if(serverMsg.equals("PLAY_AGAIN")){
				
				System.out.println("Akarsz visszavagot jatszani?");
				
				//amíg nem érvényes választást ad meg a játékos, addig olvassuk a választásait
				do{
					System.out.print("(igen/nem) ");
				}while(!isCorrectChoice((myMsg = stdIn.readLine())));
				
				// az érvényes választást elküldjük a szervernek
				socketOut.println(myMsg);
				
			}else if(serverMsg.equals("GAME_OVER")){
				
				// fejléc beolvasása, kiírása
				serverMsg = socketIn.readLine();
				System.out.println(serverMsg);
				
				// 1. játékos pontjainak beolvasása, kiírása
				serverMsg = socketIn.readLine();
				System.out.println(serverMsg);
				
				// 2. játékos pontjainak beolvasása, kiírása
				serverMsg = socketIn.readLine();
				System.out.println(serverMsg);
				
				// végeredmény beolvasása, kiírása
				serverMsg = socketIn.readLine();
				System.out.println(serverMsg);
				
				// kilépés a ciklusból
				break;
				
			}
			
		}
		
		System.out.println("Kapcsolat bontasa.");
		
		// kliens socket zárása
		socket.close();
		
	}
	
}

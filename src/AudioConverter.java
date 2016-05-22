import java.io.File;
import java.io.IOException;

import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.TargetDataLine;

public class AudioConverter {
    static final long RECORD_TIME = 10000; 	// w milisekundach
    File wavFile = new File("E:/Test/RecordAudio.wav");
 
    AudioFileFormat.Type fileType = AudioFileFormat.Type.WAVE; //ustawiamy format na wav
 
    TargetDataLine targetDataLine; // TargetDataLine jest to linia, z ktorej dane dzwieku moga byc odczytane
    
    float sampleRate;	//liczba probek na sekunde
    int sampleSizeInBits;	//liczba bitow w kazdej probce
    
    AudioConverter(float sampleRate, int sampleSizeInBits) {
    	this.sampleRate = sampleRate;
    	this.sampleSizeInBits = sampleSizeInBits;
    }

    AudioFormat getAudioFormat() {
        
        int channels = 2;  //liczba kanalow (1 - mono, 2-stereo)
        boolean signed = true; // indicates whether the data is signed or unsigned
        boolean bigEndian = true;  //sprawdza czy dane w jednej probce sa zapisane w big-endian kolejnoœci bitow)
        							//bigEndian - najbardziej znaczacy bit jest ustawiany jako pierwszy
        AudioFormat format = new AudioFormat(sampleRate, sampleSizeInBits,
                                             channels, signed, bigEndian);
        return format;
    }
 
    void start() {
        try {
            AudioFormat audioFormat = getAudioFormat();
            DataLine.Info dataLineInfo = new DataLine.Info(TargetDataLine.class, audioFormat);   //TargetDataLine.class -  klasa linii danych
            																		//format- format w ktorym chcemy zapisywac dane
            																		//DataLine dodaje funkcjonalnosci do line 
            																		//takie jak start, stop
 
            // sprawdzamy czy system wspiera DataLine
            if (!AudioSystem.isLineSupported(dataLineInfo)) {
                System.out.println("Linia nie jest wspierana");
                System.exit(0);
            }
            
            targetDataLine = (TargetDataLine) AudioSystem.getLine(dataLineInfo);
            targetDataLine.open(audioFormat);
            targetDataLine.start();   //rozpoczecie przechwytywania danych
  
            AudioInputStream audioInputStream = new AudioInputStream(targetDataLine);
 
            System.out.println("Rozpoczeto nagrywanie...");
 
            // start recording
            AudioSystem.write(audioInputStream, fileType, wavFile);
 
        } catch (LineUnavailableException ex) {
            ex.printStackTrace();
        } catch (IOException ioe) {
            ioe.printStackTrace();
        }
    }
 
    void finish() {
        targetDataLine.stop();
        targetDataLine.close();
        System.out.println("Zakonczono nagrywanie");
    }

    public static void main(String[] args) throws InterruptedException {
        final AudioConverter recorder = new AudioConverter(Float.parseFloat(args[0]), Integer.parseInt(args[1]));
        // tworzymy nowy watek, ktory czeka przez okreslony czas przed zatrzymaniem nagrywania
        Thread stopper = new Thread(new Runnable() {
            public void run() {
                try {
                    Thread.sleep(RECORD_TIME);
                } catch (InterruptedException ex) {
                    ex.printStackTrace();
                }
                recorder.finish();
            }
        });
 
        stopper.start();
        recorder.start();
        
        AudioPlayer player = new AudioPlayer();
		player.playSound();
        Thread.sleep(RECORD_TIME);
    }
}
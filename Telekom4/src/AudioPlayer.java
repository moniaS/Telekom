import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;

import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.Clip;

public class AudioPlayer {
	  
	  public void playSound() {
		    try {
		        AudioInputStream audioInputStream = AudioSystem.getAudioInputStream(new File("E:/Test/RecordAudio.wav").getAbsoluteFile());
		        Clip clip = AudioSystem.getClip();
		        clip.open(audioInputStream);
		        
		        clip.start();
		        System.out.println("Rozpoczeto odtwarzanie");
		    } catch(Exception ex) {
		        System.out.println("Blad podczas odtwarzania dzwieku");
		        ex.printStackTrace();
		    }
		}
}

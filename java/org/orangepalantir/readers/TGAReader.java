package org.orangepalantir.readers;

import javax.imageio.ImageIO;
import javax.swing.*;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.BufferedInputStream;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;

/**
 * For reading a specific subset of TGA files. Primary use is a proof of concept
 * to model a c++ after.
 *
 * User: melkor
 * Date: 2/2/14
 * Time: 5:31 PM
 */
public class TGAReader {

    final static int CHUNK = 2048;
    final static int UNCOMPRESSED_TRUE_COLOR = 2;
    /**
     * This method will attempt to open the provided path and read it as a tga file.
     * It will check the only supported format, which is uncompressed true-color, if the file
     * passed that test then a buffered image will be returned.
     *
     * @param path
     *
     * @throws IOException if the file cannot be properly read, or the 3rd byte does not indicate
     *         a valid image type.
     */
    static public BufferedImage read(Path path) throws IOException {
        BufferedInputStream stream = new BufferedInputStream(Files.newInputStream(path));
        ArrayList<byte[]> data = new ArrayList<byte[]>();
        boolean reading = true;
        int cumulative = 0;

        //read the whole file.
        while(reading){
            byte[] chunk = new byte[CHUNK];
            int read = stream.read(chunk,0, CHUNK );

            if(read<CHUNK){
                reading=false;
            }
            if(read>0){
                data.add(chunk);
                cumulative += read;
            }


        }
        stream.close();

        ByteStack stack = new ByteStack(data, cumulative, CHUNK);

        /**
         * All fields of the image are derived from the wikipedia page:
         * https://en.wikipedia.org/wiki/Truevision_TGA
         *
         */
        int footer = 26;
        int offset = 18;

        //lower left.
        int x_ll = (stack.getUnsigned(9)<<8) + stack.getUnsigned(8);
        int y_ll = (stack.getUnsigned(11)<<8) + stack.getUnsigned(10);

        //image dimensions
        int width = (stack.getUnsigned(13)<<8) + stack.getUnsigned(12);
        int height = (stack.getUnsigned(15)<<8) + stack.getUnsigned(14);

        int byte_per_pixel = stack.getUnsigned(16)/8;

        //part of the file spec. The end of the file should say TRUEVISION-XFILE.
        char[] signature = new char[17];
        int start = cumulative - footer + 8;

        for(int i = 0; i<17; i++){
            signature[i] = (char)stack.getUnsigned(start + i);
        }
        System.out.println(new String(signature));

        System.out.println("lower left: " + x_ll + "," + y_ll + "size: " + width + "x" + height);

        int type = stack.get(2);

        if(type!=UNCOMPRESSED_TRUE_COLOR){
            throw new IOException("The image type provided is not valid. Only uncompressed" +
                    "true type tga files accepted. Type provided: " + type);
        }


        BufferedImage buffy = new BufferedImage(width, height, BufferedImage.TYPE_4BYTE_ABGR);

        //comes in bgra.
        for(int i = 0; i<width; i++){
            for(int j = 0; j<height; j++){
                int dex = (j*width + i)*byte_per_pixel + offset;
                int b = stack.getUnsigned(dex);
                int g = stack.getUnsigned(dex+1);
                int r = stack.getUnsigned(dex+2);
                int a = stack.getUnsigned(dex+3);

                int y = j;

                if(y_ll==0){
                    y = height - j - 1;
                }

                buffy.setRGB(i, y, (a<<24) + (r<<16) + (g<<8) + b);
            }
        }

        return buffy;
    }

    public static void main(String[] args) {

        if(args.length==0){
            System.out.println("Usage: TGAReader image.tga");
            System.exit(0);
        }
        Path path = Paths.get(args[0]);


        try{
            BufferedImage buffy = read(path);

            ImagePanel panel = new ImagePanel(buffy);

            JFrame frame = new JFrame("TGA test");
            JScrollPane pane = new JScrollPane(panel);
            frame.add(pane);
            frame.setSize(900, 600);
            frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
            frame.setVisible(true);
        }  catch (IOException e) {
            e.printStackTrace();
        }


    }
}

/**
 * Class for drawing an image for debugging/testing purposes.
 */
class ImagePanel extends JPanel{
    Image image;
    ImagePanel(Image img){
        image = img;
        Dimension fixed = new Dimension(img.getWidth(this), img.getHeight(this));
        setSize(fixed);
        setMinimumSize(fixed);
        setMaximumSize(fixed);
        setPreferredSize(fixed);

    }
    @Override
    public void paintComponent(Graphics g){
        g.drawImage(image, 0, 0, this);
    }

}

/**
 * Utility class for loading bytes. byte[] s are loaded in chuncks and added to a
 * byte stack, then bytes from the stack are referenced as a single index.
 */
class ByteStack{
    ArrayList<byte[]> stack;
    int length;
    int chunk;
    ByteStack(ArrayList<byte[]> s, int length, int chunk){
        this.length = length;
        this.stack = s;
        this.chunk=chunk;
    }

    byte get(int dex){

        int row = dex/chunk;
        int column = dex%chunk;

        return stack.get(row)[column];

    }

    /**
     * Assumes twos compliment interpretation of an unsigned byte.
     * @param dex
     * @return
     */
    int getUnsigned(int dex){
        int row = dex/chunk;
        int column = dex%chunk;

        int b = stack.get(row)[column];
        return b<0?256 + b:b;
    }

}
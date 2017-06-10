package com.example.adamlbaker.look_for_green;

import android.graphics.Matrix;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import android.Manifest;
import android.app.Activity;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.SeekBar;

import java.io.IOException;

import static android.graphics.Color.alpha;
import static android.graphics.Color.blue;
import static android.graphics.Color.green;
import static android.graphics.Color.red;
import static android.graphics.Color.rgb;



public class MainActivity extends Activity implements TextureView.SurfaceTextureListener {

    private Camera mCamera;
    private TextureView mTextureView;
    private SurfaceView mSurfaceView;
    private SurfaceHolder mSurfaceHolder;
    Matrix matrix = new Matrix();
    private Bitmap bmp = Bitmap.createBitmap(640, 480, Bitmap.Config.ARGB_8888);
    private Canvas canvas = new Canvas(bmp);
    private Paint paint1 = new Paint();
    private TextView mTextView;
    int thresh = 140; // comparison value
    static long prevtime = 0; // for FPS calculation

    SeekBar myControl;
    TextView myTextView;

    private void setMyControlListener() {
        myControl.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {

            int progressChanged = 0;

            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                progressChanged = progress;

                thresh = progress*255/100; // sets the threshold
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

    }


    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON); // keeps the screen from turning off

        mTextView = (TextView) findViewById(R.id.cameraStatus);

        // see if the app has permission to use the camera
        ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.CAMERA}, 1);
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) == PackageManager.PERMISSION_GRANTED) {
            mSurfaceView = (SurfaceView) findViewById(R.id.surfaceview);
            mSurfaceHolder = mSurfaceView.getHolder();

            mTextureView = (TextureView) findViewById(R.id.textureview);
            mTextureView.setSurfaceTextureListener(this);

            // set the paintbrush for writing text on the image
            paint1.setColor(0xffff0000); // red
            paint1.setTextSize(24);

            mTextView.setText("started camera");
        } else {
            mTextView.setText("no camera permissions");
        }

        myControl = (SeekBar) findViewById(R.id.seekBarThresh);
        myTextView = (TextView) findViewById(R.id.SeekBarThreshText);//R.id.textSeekerBar_1_100);
        setMyControlListener();

    }

    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        mCamera = Camera.open();
        Camera.Parameters parameters = mCamera.getParameters();
        parameters.setPreviewSize(640, 480);
        parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_INFINITY); // no autofocusing
        parameters.setAutoExposureLock(true); // keep the white balance constant
        mCamera.setParameters(parameters);
        mCamera.setDisplayOrientation(90); // rotate to portrait mode

        try {
            mCamera.setPreviewTexture(surface);
            mCamera.startPreview();
        } catch (IOException ioe) {
            // Something bad happened
        }
    }

    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        // Ignored, Camera does all the work for us
    }

    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        mCamera.stopPreview();
        mCamera.release();
        return true;
    }

    // the important function
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        // every time there is a new Camera preview frame
        mTextureView.getBitmap(bmp);

        final Canvas c = mSurfaceHolder.lockCanvas();
        if (c != null) {
            int tol = 400;
            int[] pixels = new int[bmp.getWidth()]; // pixels[] is the RGBA data
            myTextView.setText(""+bmp.getWidth());
            int startY = 200; // which row in the bitmap to analyze to read
            int endY = bmp.getHeight()-200;
            for(int j = startY; j < endY; j++) {


            bmp.getPixels(pixels, 0, bmp.getWidth(), 0, j, bmp.getWidth(), 1);

            // in the row, see if there is more green than red
            for (int i = 0; i < bmp.getWidth(); i++) {
                if (green(pixels[i])+red(pixels[i])+blue(pixels[i]) < 700 &&(green(pixels[i])-red(pixels[i]))*(green(pixels[i])-red(pixels[i])) < thresh && (red(pixels[i]) - blue(pixels[i]))*(red(pixels[i]) - blue(pixels[i])) < thresh && (blue(pixels[i])-green(pixels[i]))*(blue(pixels[i])-green(pixels[i])) < thresh ) {
                //grey specific //if ((green(pixels[i])-thresh)*(green(pixels[i])-thresh) < tol && (red(pixels[i]) - thresh)*(red(pixels[i]) - thresh) < tol && (blue(pixels[i])-thresh)*(blue(pixels[i])-thresh) < tol ) {
                //white thresh =700//if (((green(pixels[i]) + red(pixels[i]) + blue(pixels[i]))) > thresh) {
                    // alpha is currently set to get rid of black, but maybe should be used to alter the threshold
                    pixels[i] = rgb(0, 255, 0); // over write the pixel with pure green
                }
            }

            // update the row
            bmp.setPixels(pixels, 0, bmp.getWidth(), 0, j, bmp.getWidth(), 1);
            }
        }

        // draw a circle at some position
        int pos = 50;
        canvas.drawCircle(pos, 240, 5, paint1); // x position, y position, diameter, color

        // write the pos as text
        canvas.drawText("pos = " + pos, 10, 200, paint1);
        c.drawBitmap(bmp, 0, 0, null);
        mSurfaceHolder.unlockCanvasAndPost(c);

        // calculate the FPS to see how fast the code is running
        long nowtime = System.currentTimeMillis();
        long diff = nowtime - prevtime;
        mTextView.setText("FPS " + 1000 / diff);
        prevtime = nowtime;
    }
}
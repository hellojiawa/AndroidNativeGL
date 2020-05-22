package com.lerp.nativegl;

import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLSurfaceView;
import android.os.Bundle;

import java.io.IOException;

import androidx.appcompat.app.AppCompatActivity;

import static android.opengl.GLSurfaceView.RENDERMODE_WHEN_DIRTY;

public class MainActivity extends AppCompatActivity implements Camera.PreviewCallback{

    public final static int WIDTH = 1920;
    public final static int HEIGHT = 1080;
    private Camera mCamera;
    private RendererWrapper mRenderer;
    private GLSurfaceView mGlSurfaceView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        mGlSurfaceView = new GLSurfaceView(this);
        setContentView(mGlSurfaceView);

        mGlSurfaceView.setEGLContextClientVersion(2);
        mRenderer = new RendererWrapper(WIDTH, HEIGHT);
        mGlSurfaceView.setRenderer(mRenderer);
        //这种模式不会重复绘制，更加省电
        mGlSurfaceView.setRenderMode(RENDERMODE_WHEN_DIRTY);
    }

    /**
     * 打开摄像头
     */
    public void openCamera(int id) {
        mCamera = Camera.open(id);
        Camera.Parameters cp = mCamera.getParameters();
        cp.setPreviewSize(WIDTH, HEIGHT);
        cp.setPictureSize(WIDTH, HEIGHT);
        cp.setFocusMode(Camera.Parameters.FOCUS_MODE_AUTO);
        try {
            mCamera.addCallbackBuffer(new byte[WIDTH * HEIGHT * 3 / 2]);
            mCamera.setPreviewCallbackWithBuffer(MainActivity.this);

            //执行这个代码onPreviewFrame才会被调用
            mCamera.setPreviewTexture(new SurfaceTexture(10));
        } catch (IOException e) {

        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        openCamera(0);
        mCamera.startPreview();
    }

    @Override
    protected void onPause() {
        super.onPause();
        mCamera.stopPreview();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mRenderer.release();
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        camera.addCallbackBuffer(data);

        mRenderer.feedData(data);
        mGlSurfaceView.requestRender();
    }
}

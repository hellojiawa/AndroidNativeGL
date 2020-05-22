package com.lerp.nativegl;

import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * TODO
 * version: V1.0 <描述当前版本功能>
 * author: liuping
 * date: 2020/5/18 16:33
 */
public class RendererWrapper implements GLSurfaceView.Renderer {

    private int mVideoWidth, mVideoHeight;
    private byte[] mData;

    public RendererWrapper(int videoWidth, int videoHeight) {
        mVideoWidth = videoWidth;
        mVideoHeight = videoHeight;
    }

    static {
        System.loadLibrary("native-lib");
    }

    public native void onSurfaceCreated(int videoWidth, int videoHeight);

    public native void onSurfaceChanged(int width, int height);

    public native void onDrawFrame(byte[] yuv);

    public native void release();

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        onSurfaceCreated(mVideoWidth, mVideoHeight);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        // No-op
        onSurfaceChanged(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        if (mData != null) {
            onDrawFrame(mData);
        }
    }

    public void feedData(byte[] yuv) {
        mData = yuv;
    }
}
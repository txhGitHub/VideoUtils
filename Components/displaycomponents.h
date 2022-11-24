#ifndef DISPLAYCOMPONENTS_H
#define DISPLAYCOMPONENTS_H

#include <QMainWindow>
#include <QObject>
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <GL/gl.h>
#include <GL/glu.h>
#include <QOpenGLTexture>

#include "videoframe.h"


/**
*  public slots:  //任意对象都可以将信号与之链接
*  protected slots： //只有本类和子类可以将信号与之链接
*  private  slots：  //在这个区内声明的槽意味着只有类自己可以将信号与之相连接
*
* explicit:不能用于隐式转换和赋值初始化
*
* initializeGL()-设置OpenGL资源和状态。在第一次调用resizeGL()或paintGL()之前调用一次
*/


class DisplayComponents : public QOpenGLWidget, protected QOpenGLFunctions
{
Q_OBJECT

public:
    DisplayComponents(QWidget *parent = nullptr);

signals:

public slots:
    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void setImage(const QImage &image);
    void initTextures();
    void initShaders();
    void setVideoWH(int w, int h);
    void showInputFrame(VideoFramePtr videoFrame);


private:
    QVector<QVector3D> vertices;
    QVector<QVector2D> texCoords;
    QOpenGLShaderProgram* m_program;
    QOpenGLTexture *texture;
    QMatrix4x4 projection;

    QOpenGLShader *m_pVSHader;  //顶点着色器程序对象
    QOpenGLShader *m_pFSHader;  //片段着色器对象

    GLuint m_posAttr;
    GLuint m_colAttr;

    QOpenGLShaderProgram *m_pShaderProgram; //着色器程序容器

    ///OPenGL用于绘制图像
    GLuint textureUniformY; //y纹理数据位置
    GLuint textureUniformU; //u纹理数据位置
    GLuint textureUniformV; //v纹理数据位置
    GLfloat *m_vertexVertices; // 顶点矩阵
    QOpenGLTexture* m_pTextureY;  //y纹理对象
    QOpenGLTexture* m_pTextureU;  //u纹理对象
    QOpenGLTexture* m_pTextureV;  //v纹理对象
    GLuint id_y; //y纹理对象ID
    GLuint id_u; //u纹理对象ID
    GLuint id_v; //v纹理对象ID
    VideoFramePtr mVideoFrame;

    int m_nVideoW; //视频分辨率宽
    int m_nVideoH; //视频分辨率高
    // QWidget interface

};

#endif // DISPLAYCOMPONENTS_H

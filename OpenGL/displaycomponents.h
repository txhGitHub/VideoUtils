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


private:
    QVector<QVector3D> vertices;
    QVector<QVector2D> texCoords;
    QOpenGLShaderProgram program;
    QOpenGLTexture *texture;
    QMatrix4x4 projection;

    // QWidget interface

};

#endif // DISPLAYCOMPONENTS_H

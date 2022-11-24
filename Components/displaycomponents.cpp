#include "displaycomponents.h"

#include <QtDebug>



#define ATTRIB_VERTEX 3
#define ATTRIB_TEXTURE 4

///用于绘制矩形
//! [3]
static const char *vertexShaderSource =
    "attribute highp vec4 posAttr;\n"
    "attribute lowp vec4 colAttr;\n"
    "varying lowp vec4 col;\n"
    "uniform highp mat4 matrix;\n"
    "void main() {\n"
    "   col = colAttr;\n"
    "   gl_Position = posAttr;\n"
    "}\n";

static const char *fragmentShaderSource =
    "varying lowp vec4 col;\n"
    "void main() {\n"
    "   gl_FragColor = col;\n"
    "}\n";
//! [3]

DisplayComponents::DisplayComponents(QWidget *parent):
     QOpenGLWidget(parent)
{
   m_vertexVertices = new GLfloat[8];

   m_nVideoH = 0;
   m_nVideoW = 0;

}

void DisplayComponents::initializeGL()
{
    initializeOpenGLFunctions(); //初始化OPenGL功能函数
       glClearColor(0, 0, 0, 0);    //设置背景为黑色
       glEnable(GL_TEXTURE_2D);     //设置纹理2D功能可用
       initTextures();              //初始化纹理设置
       initShaders();               //初始化shaders
}

void DisplayComponents::resizeGL(int w, int h)
{
    // 计算窗口横纵比
   qreal aspect = qreal(w) / qreal(h ? h : 1);
   // 设置近平面值 3.0, 远平面值 7.0, 视场45度
   const qreal zNear = 3.0, zFar = 7.0, fov = 45.0;
   // 重设投影
   projection.setToIdentity();
   // 设置透视投影
   projection.perspective(fov, static_cast<float>(aspect), zNear, zFar);
}

void DisplayComponents::paintGL()
{
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清除屏幕缓存和深度缓冲
//    QMatrix4x4 matrix;
//    matrix.translate(0.0, 0.0, -5.0);                   //矩阵变换
//    program.enableAttributeArray(0);
//    program.enableAttributeArray(1);
//    program.setAttributeArray(0, vertices.constData());
//    program.setAttributeArray(1, texCoords.constData());
//    program.setUniformValue("texture", 0); //将当前上下文中位置的统一变量设置为value
//    texture->bind();  //绑定纹理
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);//绘制纹理
//    texture->release(); //释放绑定的纹理
//    texture->destroy(); //消耗底层的纹理对象
//    texture->create();

    VideoFrame * videoFrame = mVideoFrame.get();

    if (videoFrame != nullptr)
    {
        uint8_t *m_pBufYuv420p = videoFrame->buffer();

        if (m_pBufYuv420p != NULL)
        {
            m_pShaderProgram->bind();

            //加载y数据纹理
            //激活纹理单元GL_TEXTURE0
            glActiveTexture(GL_TEXTURE0);
            //使用来自y数据生成纹理
            glBindTexture(GL_TEXTURE_2D, id_y);
            //使用内存中m_pBufYuv420p数据创建真正的y数据纹理
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW, m_nVideoH, 0, GL_RED, GL_UNSIGNED_BYTE, m_pBufYuv420p);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            //加载u数据纹理
            glActiveTexture(GL_TEXTURE1);//激活纹理单元GL_TEXTURE1
            glBindTexture(GL_TEXTURE_2D, id_u);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW/2, m_nVideoH/2, 0, GL_RED, GL_UNSIGNED_BYTE, (char*)m_pBufYuv420p+m_nVideoW*m_nVideoH);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            //加载v数据纹理
            glActiveTexture(GL_TEXTURE2);//激活纹理单元GL_TEXTURE2
            glBindTexture(GL_TEXTURE_2D, id_v);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_nVideoW/2, m_nVideoH/2, 0, GL_RED, GL_UNSIGNED_BYTE, (char*)m_pBufYuv420p+m_nVideoW*m_nVideoH*5/4);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            //指定y纹理要使用新值 只能用0,1,2等表示纹理单元的索引，这是opengl不人性化的地方
            //0对应纹理单元GL_TEXTURE0 1对应纹理单元GL_TEXTURE1 2对应纹理的单元
            glUniform1i(textureUniformY, 0);
            //指定u纹理要使用新值
            glUniform1i(textureUniformU, 1);
            //指定v纹理要使用新值
            glUniform1i(textureUniformV, 2);
            //使用顶点数组方式绘制图形
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

            m_pShaderProgram->release();
        }
    }
}

void DisplayComponents::setImage(const QImage &image)
{

    texture->setData(image); //设置纹理图像
        //设置纹理细节
    texture->setLevelofDetailBias(0);//值越小，图像越清晰
//    qDebug() << "架构" ;
    update();
}

void DisplayComponents::initTextures()
{
    // 加载 Avengers.jpg 图片
        texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
        texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        texture->setMagnificationFilter(QOpenGLTexture::Linear);
        //重复使用纹理坐标
        //纹理坐标(1.1, 1.2)与(0.1, 0.2)相同
        texture->setWrapMode(QOpenGLTexture::Repeat);
        //设置纹理大小
        texture->setSize(this->width(), this->height());
        //分配储存空间
        texture->allocateStorage();
}

//初始化的是与RGB图像相关
//void DisplayComponents::initShaders()
//{
//    //纹理坐标
//    texCoords.append(QVector2D(0, 1)); //左上
//    texCoords.append(QVector2D(1, 1)); //右上
//    texCoords.append(QVector2D(0, 0)); //左下
//    texCoords.append(QVector2D(1, 0)); //右下
//    //顶点坐标
//    vertices.append(QVector3D(-1, -1, 1));//左下
//    vertices.append(QVector3D(1, -1, 1)); //右下
//    vertices.append(QVector3D(-1, 1, 1)); //左上
//    vertices.append(QVector3D(1, 1, 1));  //右上

//    QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
//    const char *vsrc =
//           "attribute vec4 vertex;\n"
//           "attribute vec2 texCoord;\n"
//           "varying vec2 texc;\n"
//           "void main(void)\n"
//           "{\n"
//           "    gl_Position = vertex;\n"
//           "    texc = texCoord;\n"
//           "}\n";
//    vshader->compileSourceCode(vsrc);//编译顶点着色器代码

//    QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
//    const char *fsrc =
//           "uniform sampler2D texture;\n"
//           "varying vec2 texc;\n"
//           "void main(void)\n"
//           "{\n"
//           "    gl_FragColor = texture2D(texture,texc);\n"
//           "}\n";
//    fshader->compileSourceCode(fsrc); //编译纹理着色器代码
//    program.addShader(vshader);//添加顶点着色器
//   program.addShader(fshader);//添加纹理碎片着色器
//   program.bindAttributeLocation("vertex", 0);//绑定顶点属性位置
//   program.bindAttributeLocation("texCoord", 1);//绑定纹理属性位置
//   // 链接着色器管道
//   if (!program.link())
//       close();
//   // 绑定着色器管道
//   if (!program.bind())
//       close();
//}

void DisplayComponents::initShaders()
{
     m_pVSHader = new QOpenGLShader(QOpenGLShader::Vertex, this);
     //顶点着色器源码
     const char *vsrc = "attribute vec4 vertexIn; \
     attribute vec2 textureIn; \
     varying vec2 textureOut;  \
     void main(void)           \
     {                         \
         gl_Position = vertexIn; \
         textureOut = textureIn; \
     }";

     //编译顶点着色器程序
     bool bCompile = m_pVSHader->compileSourceCode(vsrc);
     if(!bCompile)
     {
     }

     //初始化片段着色器 功能gpu中yuv转换成rgb
     m_pFSHader = new QOpenGLShader(QOpenGLShader::Fragment, this);
     const char *fsrc =
 #if defined(WIN32)
     "#ifdef GL_ES\n"
     "precision mediump float;\n"
     "#endif\n"
 #else
 #endif
     "varying vec2 textureOut; \
     uniform sampler2D tex_y; \
     uniform sampler2D tex_u; \
     uniform sampler2D tex_v; \
     void main(void) \
     { \
         vec3 yuv; \
         vec3 rgb; \
         yuv.x = texture2D(tex_y, textureOut).r; \
         yuv.y = texture2D(tex_u, textureOut).r - 0.5; \
         yuv.z = texture2D(tex_v, textureOut).r - 0.5; \
         rgb = mat3( 1,       1,         1, \
                     0,       -0.39465,  2.03211, \
                     1.13983, -0.58060,  0) * yuv; \
         gl_FragColor = vec4(rgb, 1); \
     }";

     //将glsl源码送入编译器编译着色器程序
     bCompile = m_pFSHader->compileSourceCode(fsrc);
     if(!bCompile)
     {
     }

     ///用于绘制矩形
     m_program = new QOpenGLShaderProgram(this);
     m_program->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
     m_program->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
     m_program->link();
     m_posAttr = m_program->attributeLocation("posAttr");
     m_colAttr = m_program->attributeLocation("colAttr");

#define PROGRAM_VERTEX_ATTRIBUTE 0
#define PROGRAM_TEXCOORD_ATTRIBUTE 1
     //创建着色器程序容器
     m_pShaderProgram = new QOpenGLShaderProgram;
     //将片段着色器添加到程序容器
     m_pShaderProgram->addShader(m_pFSHader);
     //将顶点着色器添加到程序容器
     m_pShaderProgram->addShader(m_pVSHader);
     m_pShaderProgram->bindAttributeLocation("vertexIn", ATTRIB_VERTEX);
     //绑定属性textureIn到指定位置ATTRIB_TEXTURE,该属性在顶点着色源码其中有声明
     m_pShaderProgram->bindAttributeLocation("textureIn", ATTRIB_TEXTURE);
     //链接所有所有添入到的着色器程序
     m_pShaderProgram->link();
     //激活所有链接
     m_pShaderProgram->bind();
     textureUniformY = m_pShaderProgram->uniformLocation("tex_y");
     textureUniformU =  m_pShaderProgram->uniformLocation("tex_u");
     textureUniformV =  m_pShaderProgram->uniformLocation("tex_v");

     // 顶点矩阵
     const GLfloat vertexVertices[] = {
         -1.0f, -1.0f,
          1.0f, -1.0f,
          -1.0f, 1.0f,
          1.0f, 1.0f,
     };

     memcpy(m_vertexVertices, vertexVertices, sizeof(vertexVertices));

     //纹理矩阵
     static const GLfloat textureVertices[] = {
         0.0f,  1.0f,
         1.0f,  1.0f,
         0.0f,  0.0f,
         1.0f,  0.0f,
     };
     ///设置读取的YUV数据为1字节对其，默认4字节对齐，
     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

     //设置属性ATTRIB_VERTEX的顶点矩阵值以及格式
     glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, m_vertexVertices);
     //设置属性ATTRIB_TEXTURE的纹理矩阵值以及格式
     glVertexAttribPointer(ATTRIB_TEXTURE, 2, GL_FLOAT, 0, 0, textureVertices);
     //启用ATTRIB_VERTEX属性的数据,默认是关闭的
     glEnableVertexAttribArray(ATTRIB_VERTEX);
     //启用ATTRIB_TEXTURE属性的数据,默认是关闭的
     glEnableVertexAttribArray(ATTRIB_TEXTURE);
     //分别创建y,u,v纹理对象
     m_pTextureY = new QOpenGLTexture(QOpenGLTexture::Target2D);
     m_pTextureU = new QOpenGLTexture(QOpenGLTexture::Target2D);
     m_pTextureV = new QOpenGLTexture(QOpenGLTexture::Target2D);
     m_pTextureY->create();
     m_pTextureU->create();
     m_pTextureV->create();
     //获取返回y分量的纹理索引值
     id_y = m_pTextureY->textureId();
     //获取返回u分量的纹理索引值
     id_u = m_pTextureU->textureId();
     //获取返回v分量的纹理索引值
     id_v = m_pTextureV->textureId();
     //qDebug("addr=%x id_y = %d id_u=%d id_v=%d\n", this, id_y, id_u, id_v);
}

void DisplayComponents::setVideoWH(int w, int h)
{
    if (w <= 0 || h <= 0) return;
    m_nVideoW = w;
    m_nVideoH = h;
}

void DisplayComponents::showInputFrame(std::shared_ptr<VideoFrame> videoFrame)
{
    int width = videoFrame.get()->getWidth();
    int height = videoFrame.get()->getHeight();

    if (m_nVideoW <= 0 || m_nVideoH <= 0 || m_nVideoW != width || m_nVideoH != height)
    {
        setVideoWH(width, height);
    }
    mVideoFrame.reset();
    mVideoFrame = videoFrame;
    update(); //调用update将执行 paintEvent函数
}

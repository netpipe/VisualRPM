#include <QApplication>
#include <QWidget>
#include <QSlider>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>
#include <QPainter>
#include <QPixmap>
#include <QTransform>
#include <QIntValidator>

// Custom widget that displays a rotating image
class RotatingImage : public QLabel {
    Q_OBJECT
public:
    explicit RotatingImage(QWidget *parent = nullptr)
        : QLabel(parent), angle(0), rpm(0)
    {
        // Load a simple placeholder image (you can replace this with your own)
        image = QPixmap(100, 100);
        image.fill(Qt::transparent);
        QPainter p(&image);
        p.setRenderHint(QPainter::Antialiasing);
        p.setPen(Qt::black);
        p.setBrush(Qt::gray);
        p.drawEllipse(10, 10, 80, 80);
        p.setBrush(Qt::black);
        p.drawRect(48, 0, 4, 50);
        p.end();

        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &RotatingImage::updateRotation);
        timer->start(16); // roughly 60 fps
    }

    void setRPM(int newRPM) {
        rpm = newRPM;
    }

protected:
    void paintEvent(QPaintEvent *) override {
        QPainter painter(this);
        QTransform transform;
        transform.translate(width() / 2.0, height() / 2.0);
        transform.rotate(angle);
        transform.translate(-image.width() / 2.0, -image.height() / 2.0);
        painter.setTransform(transform);
        painter.drawPixmap(0, 0, image);
    }

private slots:
    void updateRotation() {
        // degrees per frame = (rpm * 360 degrees per revolution) / (60 seconds * fps)
        double degreesPerFrame = (rpm * 360.0) / (60.0 * 60.0); // assuming ~60 fps
        angle += degreesPerFrame;
        if (angle >= 360.0)
            angle -= 360.0;
        update();
    }

private:
    QPixmap image;
    QTimer *timer;
    double angle;
    int rpm;
};

// Main window setup
class RPMSimulator : public QWidget {
    Q_OBJECT
public:
    RPMSimulator() {
        auto *layout = new QVBoxLayout(this);

        // Custom rotating image
        rotator = new RotatingImage(this);
        rotator->setFixedSize(150, 150);
        layout->addWidget(rotator, 0, Qt::AlignCenter);

        // Slider
        auto *sliderLayout = new QHBoxLayout();
        QLabel *label = new QLabel("RPM:", this);
        slider = new QSlider(Qt::Horizontal, this);
        slider->setRange(1, 1000);
        slider->setValue(100);
        sliderLayout->addWidget(label);
        sliderLayout->addWidget(slider);
        layout->addLayout(sliderLayout);

        // Custom RPM input box
        input = new QLineEdit(this);
        input->setPlaceholderText("Enter custom RPM and press Enter");
        input->setValidator(new QIntValidator(1, 1000, this));
        layout->addWidget(input);

        connect(slider, &QSlider::valueChanged, this, &RPMSimulator::onSliderChanged);
        connect(input, &QLineEdit::returnPressed, this, &RPMSimulator::onInputEntered);

        onSliderChanged(slider->value());
    }

private slots:
    void onSliderChanged(int value) {
        rotator->setRPM(value);
        input->setText(QString::number(value));
    }

    void onInputEntered() {
        int value = input->text().toInt();
        if (value >= 1 && value <= 1000) {
            slider->setValue(value);
        }
    }

private:
    RotatingImage *rotator;
    QSlider *slider;
    QLineEdit *input;
};

#include "main.moc"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    RPMSimulator window;
    window.setWindowTitle("RPM Simulator - Qt 5.12");
    window.resize(300, 300);
    window.show();
    return app.exec();
}

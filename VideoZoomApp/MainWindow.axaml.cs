using Avalonia;
using Avalonia.Controls;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Markup.Xaml;
using OpenCvSharp;
using Avalonia.Media.Imaging;
using System.IO;
using Avalonia.Input;
using System.Threading.Tasks;
using Avalonia.Platform;

namespace VideoZoomApp
{
    public partial class MainWindow : Avalonia.Controls.Window
    {
        private VideoCapture? _capture;
        private TextBlock _logTextBlock;

        public MainWindow()
        {
            InitializeComponent();
#if DEBUG
            this.AttachDevTools();
#endif
            _logTextBlock = this.FindControl<TextBlock>("LogTextBlock");
            StartVideo();
        }

        private void InitializeComponent()
        {
            AvaloniaXamlLoader.Load(this);
        }

        private async void StartVideo()
        {
            _capture = new VideoCapture(0); // 0 for default camera

            if (!_capture.IsOpened())
            {
                LogMessage("No video devices found.");
                return;
            }

            using (Mat image = new Mat())
            {
                while (true)
                {
                    _capture.Read(image);
                    if (image.Empty())
                    {
                        break;
                    }

                    ((Image)this.FindControl<Image>("VideoView")).Source = ConvertMatToWriteableBitmap(image);

                    await Task.Delay(33); // ~30 frames per second
                }
            }
        }

        private void LogMessage(string message)
        {
            _logTextBlock.Text += message + "\n";
        }

        private static WriteableBitmap ConvertMatToWriteableBitmap(Mat image)
        {
            image.ConvertTo(image, MatType.CV_8U);
            var writableBitmap = new WriteableBitmap(
                new PixelSize(image.Width, image.Height),
                new Vector(96, 96),
                Avalonia.Platform.PixelFormat.Rgba8888,
                Avalonia.Platform.AlphaFormat.Premul
            );

            using (var buf = writableBitmap.Lock())
                {
                    unsafe
                    {
                        System.Buffer.MemoryCopy((void*)image.Data, (void*)buf.Address, image.Rows * image.Cols * 4, image.Rows * image.Cols * 4);
                    }
                }


            return writableBitmap;
        }
    }
}

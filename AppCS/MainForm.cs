using System;
using System.Drawing;
using System.Windows.Forms;
using AForge.Video;
using AForge.Video.DirectShow;
using System.IO;

public class MainForm : Form
{
    private FilterInfoCollection CaptureDevice;
    private VideoCaptureDevice FinalFrame;
    private Bitmap currentFrame;
    private bool showingVideo = true;
    private PictureBox pictureBox;
    private Button takePictureButton;
    private Button uploadPictureButton;
    private Button returnToVideoButton;
    private Button closeButton;
    private OpenFileDialog openFileDialog;
    private Panel panel;

    public MainForm()
    {
        InitializeComponent();
        CaptureDevice = new FilterInfoCollection(FilterCategory.VideoInputDevice);
        FinalFrame = new VideoCaptureDevice();
        FinalFrame = new VideoCaptureDevice(CaptureDevice[0].MonikerString);
        FinalFrame.NewFrame += new NewFrameEventHandler(FinalFrame_NewFrame);
        FinalFrame.Start();
    }
    protected override void OnFormClosing(FormClosingEventArgs e)
    {
        base.OnFormClosing(e);
        if (FinalFrame != null && FinalFrame.IsRunning)
        {
            FinalFrame.SignalToStop();
            FinalFrame.WaitForStop();
        }
    }
    private void InitializeComponent()
    {
        this.panel = new Panel();
        this.pictureBox = new PictureBox();
        this.takePictureButton = new Button();
        this.uploadPictureButton = new Button();
        this.returnToVideoButton = new Button();
        this.closeButton = new Button();
        this.openFileDialog = new OpenFileDialog();

        // Initialize Panel
        this.panel.Dock = DockStyle.Fill;

        // Initialize PictureBox
        this.pictureBox.Dock = DockStyle.Fill;

        // Initialize takePictureButton
        this.takePictureButton.Text = "Take Picture";
        this.takePictureButton.Dock = DockStyle.Bottom;
        this.takePictureButton.Click += new EventHandler(TakePictureButton_Click);

        // Initialize uploadPictureButton
        this.uploadPictureButton.Text = "Upload Picture";
        this.uploadPictureButton.Dock = DockStyle.Bottom;
        this.uploadPictureButton.Click += new EventHandler(UploadPictureButton_Click);

        // Initialize returnToVideoButton
        this.returnToVideoButton.Text = "Return to Video";
        this.returnToVideoButton.Dock = DockStyle.Bottom;
        this.returnToVideoButton.Click += new EventHandler(ReturnToVideoButton_Click);

        // Initialize closeButton
        this.closeButton.Text = "Close";
        this.closeButton.Dock = DockStyle.Bottom;
        this.closeButton.Click += new EventHandler(CloseButton_Click);

        // Add controls to the panel
        this.panel.Controls.Add(this.pictureBox);

        // Add controls to the form
        this.Controls.Add(this.panel);
        this.Controls.Add(this.takePictureButton);
        this.Controls.Add(this.uploadPictureButton);
        this.Controls.Add(this.returnToVideoButton);
        this.Controls.Add(this.closeButton);

        // Add key press event to the form
        this.KeyPress += new KeyPressEventHandler(MainForm_KeyPress);
        this.KeyPreview = true;
    }

    private void FinalFrame_NewFrame(object sender, NewFrameEventArgs eventArgs)
    {
        if (showingVideo)
        {
            currentFrame = (Bitmap)eventArgs.Frame.Clone();
            pictureBox.Image = currentFrame;
        }
    }

    private void TakePictureButton_Click(object sender, EventArgs e)
    {
        if (currentFrame != null)
        {
            string picturePath = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.MyPictures), "snapshot.jpg");
            currentFrame.Save(picturePath, System.Drawing.Imaging.ImageFormat.Jpeg);
            MessageBox.Show($"Snapshot saved to {picturePath}");
        }
    }

    private void UploadPictureButton_Click(object sender, EventArgs e)
    {
        DialogResult result = openFileDialog.ShowDialog();
        if (result == DialogResult.OK)
        {
                    pictureBox.Image = new Bitmap(openFileDialog.FileName);
            showingVideo = false;
        }
    }

    private void ReturnToVideoButton_Click(object sender, EventArgs e)
    {
        showingVideo = true;
    }

     private void CloseButton_Click(object sender, EventArgs e)
    {
        this.Close(); // This will trigger OnFormClosing which will close the camera properly
    }

    private void MainForm_KeyPress(object sender, KeyPressEventArgs e)
    {
        if (e.KeyChar == 'q' || e.KeyChar == 'Q')
        {
            this.Close();
        }
    }
}

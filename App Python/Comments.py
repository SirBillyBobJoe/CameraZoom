import cv2  # Import OpenCV library for computer vision tasks
from datetime import datetime  # Import datetime module to work with timestamps
import os  # Import os module for operating system related tasks
import threading  # Import threading module for multi-threading support
from kivy.app import App  # Import App class from Kivy framework for creating Kivy applications
from kivy.uix.image import Image  # Import Image class from Kivy for displaying images
from kivy.graphics.texture import Texture  # Import Texture class from Kivy for working with textures
from kivy.uix.spinner import Spinner  # Import Spinner class from Kivy for creating dropdown menus
from kivy.uix.floatlayout import FloatLayout  # Import FloatLayout class from Kivy for organizing widgets
from kivy.core.window import Window  # Import Window class from Kivy for window-related tasks
from kivy.uix.scatter import Scatter  # Import Scatter class from Kivy for creating scalable and rotatable widgets
from kivy.clock import Clock  # Import Clock class from Kivy for scheduling tasks
from kivy.uix.button import Button  # Import Button class from Kivy for creating buttons
from kivy.uix.relativelayout import RelativeLayout  # Import RelativeLayout class from Kivy for organizing widgets
from kivy.uix.filechooser import FileChooserIconView  # Import FileChooserIconView class from Kivy for file selection
from kivy.uix.popup import Popup  # Import Popup class from Kivy for creating pop-up windows
from kivy.config import Config  # Import Config class from Kivy for configuring the application
import socket  # Import socket module for networking tasks
from kivy.uix.label import Label  # Import Label class from Kivy for displaying text

Config.set('input', 'mouse', 'mouse,multitouch_on_demand')  # Disable multi-touch

# Custom class KivyCamera, extends Image class
class KivyCamera(Image):
    def __init__(self, capture, fps, **kwargs):
        super(KivyCamera, self).__init__(**kwargs)
        self.capture = capture  # Video capture object
        self.fps = fps  # Frames per second
        self.event = None  # Event for scheduling frame updates
        self.allow_stretch = True  # Allow the image to be resized
        self.keep_ratio = True  # Allow the aspect ratio to be changed
        self.start()  # Start capturing frames

    def start(self):
        if self.event is None:
            self.event = Clock.schedule_interval(self.update, 1.0 / self.fps)  # Schedule frame updates

    def stop(self):
        if self.event is not None:
            self.event.cancel()  # Cancel frame updates
            self.event = None

    def update(self, dt):
        ret, frame = self.capture.read()  # Read frame from video capture
        if ret:
            # Convert frame to texture
            buf1 = cv2.flip(frame, 0)  # Flip frame vertically
            buf = buf1.tostring()
            image_texture = Texture.create(size=(frame.shape[1], frame.shape[0]), colorfmt='bgr')
            image_texture.blit_buffer(buf, colorfmt='bgr', bufferfmt='ubyte')
            # Display image from the texture
            self.texture = image_texture
        else:
            print("Error: Unable to read frame from camera")

    @staticmethod
    def get_available_cameras(max_cameras=10):
        cameras = []
        for i in range(max_cameras):
            cap = cv2.VideoCapture(i)  # Attempt to open video capture for each camera index
            if cap is None or not cap.isOpened():
                cap.release()  # Release video capture
                break
            else:
                cameras.append(i)  # Add camera index to the list of available cameras
                cap.release()  # Release video capture
        return cameras

    @staticmethod
    def get_supported_resolutions(cam_id):
        common_resolutions = ['640x480', '1024x768', '1280x720', '1920x1080']
        supported_resolutions = []

        cap = cv2.VideoCapture(cam_id)  # Open video capture for the specified camera index
        if cap.isOpened():
            for res in common_resolutions:
                width, height = map(int, res.split('x'))
                cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)  # Set frame width
                cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)  # Set frame height
                actual_resolution = (cap.get(cv2.CAP_PROP_FRAME_WIDTH), cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
                if actual_resolution == (width, height):
                    supported_resolutions.append(res)  # Add supported resolution to the list
        cap.release()  # Release video capture

        return supported_resolutions if supported_resolutions else ['Default']


# Custom class CustomScatter, extends Scatter class
class CustomScatter(Scatter):
    def on_transform_with_touch(self, touch):
        # Override Scatter's method to prevent it from changing the widget order
        pass


# Custom class ScatterImage, extends Scatter and Image classes
class ScatterImage(Scatter, Image):
    def __init__(self, **kwargs):
        super(ScatterImage, self).__init__(**kwargs)
        self.allow_stretch = True  # Allow the image to be resized
        self.keep_ratio = True  # Allow the aspect ratio to be changed
        self.do_translation = True  # Enable translation
        self.do_rotation = False  # Disable rotation
        self.do_scale = True  # Enable scaling

    def on_touch_down(self, touch):
        if self.collide_point(*touch.pos):
            return super(ScatterImage, self).on_touch_down(touch)
        return False

    def on_touch_move(self, touch):
        if self.collide_point(*touch.pos):
            return super(ScatterImage, self).on_touch_move(touch)
        return False

    def on_transform_with_touch(self, touch):
        super(ScatterImage, self).on_transform_with_touch(touch)

        # Limit the max and min scale
        if self.scale < 1:
            self.scale = 1
        elif self.scale > 3:
            self.scale = 3

        # Ensure the image never goes out of the screen
        if self.x > 0:
            self.x = 0
        elif self.right < self.parent.width:
            self.x = self.parent.width - self.width * self.scale

        if self.y > 0:
            self.y = 0
        elif self.top < self.parent.height:
            self.y = self.parent.height - self.height * self.scale

    def on_size(self, *args):
        if self.parent is not None:
            self.width = self.parent.width
            self.height = self.parent.height


# Main application class MainApp, extends App class from Kivy
class MainApp(App):
    def build(self):
        root = FloatLayout()  # Create the root layout for the application
        self.capture = cv2.VideoCapture(0)  # Open video capture for default camera (index 0)
        self.my_camera = KivyCamera(capture=self.capture, fps=15)  # Create KivyCamera object
        self.scatter = CustomScatter(do_rotation=False, do_translation=True)  # Create CustomScatter object
        self.scatter.size = root.size  # Set size of scatter widget to match root layout
        self.scatter.pos = root.pos  # Set position of scatter widget to match root layout
        self.scatter.add_widget(self.my_camera)  # Add KivyCamera widget to the scatter widget

        relative_layout_video = RelativeLayout()  # Create RelativeLayout for video display
        relative_layout_video.add_widget(self.scatter)  # Add scatter widget to the video layout

        self.capture_button = Button(text='Capture', size_hint=(None, None), size=(100, 50),
                                     pos_hint={'x': 0, 'y': 0})  # Create capture button
        self.capture_button.bind(on_release=self.capture_screenshot)  # Bind capture_screenshot function to button release event
        self.capture_button.opacity = 1  # Make capture button visible
        self.capture_button.disabled = False  # Enable capture button

        relative_layout_button = RelativeLayout()  # Create RelativeLayout for buttons
        relative_layout_button.add_widget(self.capture_button)  # Add capture button to the button layout

        root.add_widget(relative_layout_video)  # Add video layout to the root layout
        root.add_widget(relative_layout_button)  # Add button layout to the root layout

        self.upload_button = Button(text='Upload', size_hint=(None, None), size=(100, 50),
                                    pos_hint={'x': 0.2, 'y': 0})  # Create upload button
        self.upload_button.bind(on_release=self.open_filechooser)  # Bind open_filechooser function to button release event

        relative_layout_button.add_widget(self.upload_button)  # Add upload button to the button layout

        hostname = socket.gethostname()
        IPAddr = socket.gethostbyname(hostname)

        # Display IP address in top left-hand corner of the screen in a label
        self.ip_label = Label(text="IP Address: " + IPAddr, size_hint=(None, None), size=(100, 50),
                              pos_hint={'x': 0.045, 'y': 0.94})
        relative_layout_button.add_widget(self.ip_label)  # Add IP address label to the button layout

        self.spinner = Spinner(
            text='Camera 0',
            values=["Camera " + str(i) for i in KivyCamera.get_available_cameras()],  # Get available cameras

            size_hint=(None, None),
            size=(100, 50),
            pos_hint={'x': 0.6, 'y': 0})  # Create spinner for selecting cameras

        self.spinner.bind(text=self.on_spinner_select)  # Bind on_spinner_select function to spinner selection event
        root.add_widget(self.spinner)  # Add spinner to the root layout

        self.resolutions = ['640x480', '1280x720', '1920x1080']  # Available resolutions
        self.resolution_spinner = Spinner(
            text='Select Resolution',
            values=self.resolutions,
            size_hint=(None, None),
            size=(150, 50),
            pos_hint={'x': 0.75, 'y': 0})  # Create spinner for selecting resolutions

        self.resolution_spinner.bind(text=self.on_resolution_select)  # Bind on_resolution_select function to spinner selection event
        root.add_widget(self.resolution_spinner)  # Add resolution spinner to the root layout
        self.selected_camera = 0  # Default selected camera index

        def update_scatter_size(instance, value):
            self.scatter.size = value  # Update scatter size to match root layout
            self.my_camera.size = value  # Update camera size to match scatter widget size

        root.bind(size=update_scatter_size)  # Bind update_scatter_size function to root layout size change event

        def on_touch_down(window, touch):
            if touch.is_double_tap:
                self.scatter.scale = 1  # Reset scatter scale to 1
                self.scatter.pos = root.pos  # Reset scatter position to match root layout position
            return False  # Continue processing the event.

        def on_touch_move(window, touch):
            if len(touch.grab_list) > 0:
                # Check if there is any ongoing touch grab, indicating an active touch interaction.
                if touch.is_mouse_scrolling:
                    # If the touch interaction is a mouse scroll event.
                    if touch.button == 'scrolldown':
                        # Check if the scroll direction is down.
                        if self.scatter.scale > 1:
                            # Check if the current scale of the scatter widget is greater than 1.
                            self.scatter.scale = self.scatter.scale * 0.9
                            # Scale down the scatter widget by multiplying the scale by 0.9.
                    elif touch.button == 'scrollup':
                        # Check if the scroll direction is up.
                        self.scatter.scale = self.scatter.scale * 1.1
                        # Scale up the scatter widget by multiplying the scale by 1.1.
                elif touch.grab_current is self.scatter:
                    # If the current touch grab is the scatter widget itself.
                    if len(self.scatter._touches) == 1:
                        # Check if there is one active touch (one finger swipe).
                        touch1 = self.scatter._touches[0]
                        # Get the first touch object.
                        new_x = self.scatter.x + touch1.dx
                        new_y = self.scatter.y + touch1.dy
                        # Calculate the new x and y positions of the scatter widget
                        # by adding the change in x (dx) and y (dy) of the touch to the current x and y positions.
                        new_x = max(min(new_x, 0), self.scatter.width - self.scatter.width * self.scatter.scale)
                        new_y = max(min(new_y, 0), self.scatter.height - self.scatter.height * self.scatter.scale)
                        # Limit the new x and y positions to ensure the scatter widget does not go beyond its boundaries.
                        self.scatter.pos = (new_x, new_y)
                        # Update the scatter position with the new x and y positions.
                    elif len(self.scatter._touches) == 2:
                        # Check if there are two active touches (two finger pinch).
                        touch1, touch2 = self.scatter._touches
                        # Get the first and second touch objects.
                        self.scatter.center = ((touch1.x + touch2.x) / 2, (touch1.y + touch2.y) / 2)
                        # Set the center of the scatter widget at the midpoint of the two touches.
                        self.scatter.scale = ((touch1.dx**2 + touch1.dy**2)**0.5 +
                                            (touch2.dx**2 + touch2.dy**2)**0.5) / self.scatter.width
                        # Calculate the scale based on the distance between the two touches.
                        if self.scatter.scale < 1:
                            self.scatter.scale = 1
                            # Reset the scale to 1 if it is less than 1 to prevent scaling below 1.
            return True
            # Return True to indicate that the touch move event was handled.

        def on_transform_with_touch(scatter, touch):
            # Limit the panning to the edge of the image.
            if scatter.x > 0:
                scatter.x = 0
            if scatter.y > 0:
                scatter.y = 0
            if scatter.right < scatter.width:  # Use scatter.right which gives the x-coordinate of the right side of the scatter widget
                scatter.right = scatter.width
            if scatter.top < scatter.height:  # Use scatter.top which gives the y-coordinate of the top side of the scatter widget
                scatter.top = scatter.height

            # Prevent zooming out past the original size
            if scatter.scale < 1:
                scatter.scale = 1

        Window.bind(on_touch_down=on_touch_down)  # Bind on_touch_down function to window touch down event
        Window.bind(on_touch_move=on_touch_move)  # Bind on_touch_move function to window touch move event
        self.scatter.bind(on_transform_with_touch=on_transform_with_touch)  # Bind on_transform_with_touch function to scatter transform with touch event

        return root  # Return the root layout as the application's main widget

    def capture_screenshot(self, *args):
        # Hide buttons
        self.capture_button.opacity = 0
        self.upload_button.opacity = 0
        self.spinner.opacity = 0  # Make spinner invisible
        self.resolution_spinner.opacity = 0  # Make resolution spinner invisible
        self.resolution_spinner.disabled = True  # Disable resolution spinner

        # Set a delay of 1 second before capturing the screenshot
        Clock.schedule_once(self.capture_screenshot_after_delay, 0.1)

    def capture_screenshot_after_delay(self, *args):
        # Capture screenshot
        ret, frame = self.capture.read()  # Read frame from video capture
        if ret:
            timestamp = datetime.now().strftime('%Y%m%d%H%M%S')  # Get current timestamp
            filename = f'screenshot_{timestamp}.png'  # Create a filename with timestamp
            is_written = Window.screenshot(name=filename)  # Save the screenshot
            print(f'Screenshot saved as {filename}') if is_written else print('Error in saving screenshot')

        # Show buttons again
        self.capture_button.opacity = 1
        self.upload_button.opacity = 1
        self.spinner.opacity = 1  # Make spinner visible
        self.resolution_spinner.opacity = 1  # Make resolution spinner visible
        self.resolution_spinner.disabled = False  # Enable resolution spinner

    def open_filechooser(self, *args):
        content = FileChooserIconView(path=os.getcwd())  # Create file chooser with current directory as the starting path
        content.bind(on_submit=self.load_image)  # Bind load_image function to file chooser submission event
        popup = Popup(title="Select an image", content=content, size_hint=(0.9, 0.9))  # Create a popup window with the file chooser
        popup.open()  # Open the popup window

    def load_image(self, instance, value, *args):
        try:
            self.my_camera.stop()  # Stop capturing frames from the camera
            self.capture.release()  # Release the video capture
            self.scatter.clear_widgets()  # Clear all child widgets from the scatter widget
            scatter_image = ScatterImage(source=value[0], size=self.scatter.size, pos=self.scatter.pos)  # Create ScatterImage with selected image
            self.scatter.add_widget(scatter_image)  # Add ScatterImage to the scatter widget

            # Hide and disable capture button, show and enable live video button
            self.capture_button.opacity = 0
            self.capture_button.disabled = True
            # Disable spinner and resolution button
            self.resolution_spinner.opacity = 0
            self.resolution_spinner.disabled = True

            # Reset spinner text to 'Select Camera'
            self.spinner.text = 'Select Camera'

        except Exception as e:
            print(f"Failed to load image: {e}")

    def on_spinner_select(self, spinner, text):
        if text == 'Select Camera':
            # If 'Select Camera' is selected, don't try to start a new camera.
            return
        else:
            self.selected_camera = int(text.split()[1])  # Update selected camera index
            self.scatter.clear_widgets()  # Clear all child widgets from the scatter widget
            self.my_camera.stop() if self.my_camera else None  # Stop the previous camera if exists
            self.capture.release() if self.capture else None  # Release the previous video capture if exists
            self.capture = cv2.VideoCapture(self.selected_camera)  # Open video capture for the selected camera
            if not self.capture.isOpened():
                print(f"Error: Unable to open camera {self.selected_camera}")
                return
            self.my_camera = KivyCamera(capture=self.capture, fps=15)  # Create new KivyCamera object
            self.my_camera.allow_stretch = True  # Enable stretching the video to fill the screen
            self.my_camera.keep_ratio = True  # Enable maintaining the aspect ratio
            self.my_camera.size = self.scatter.size  # Set the size of the camera to match the scatter widget
            self.my_camera.pos = self.scatter.pos  # Set the position of the camera to match the scatter widget
            self.scatter.add_widget(self.my_camera)  # Add the camera widget to the scatter widget
            self.my_camera.start()  # Start capturing frames from the camera
            self.scatter.scale = 1  # Reset scatter scale to 1
            self.scatter.pos = self.root.pos  # Reset scatter position to match root layout position

            # Show and enable capture button, hide and disable live video button
            self.capture_button.opacity = 1
            self.capture_button.disabled = False
            # Bring back spinner and resolution button
            self.resolution_spinner.opacity = 1
            self.resolution_spinner.disabled = False

    def on_resolution_select(self, spinner, text):
        # Get new resolution parameters
        width, height = map(int, text.split('x'))

        # Function to be executed in a separate thread
        def change_resolution():
            # Create a new capture instance with the desired resolution
            new_capture = cv2.VideoCapture(self.selected_camera)
            new_capture.set(cv2.CAP_PROP_FRAME_WIDTH, width)  # Set frame width
            new_capture.set(cv2.CAP_PROP_FRAME_HEIGHT, height)  # Set frame height

            # Wait until the new capture is ready
            while not new_capture.isOpened():
                pass

            # Now, stop the old capture and replace it with the new one
            self.my_camera.stop()
            self.capture.release()
            self.my_camera.capture = new_capture
            self.capture = new_capture
            self.my_camera.start()

            # Check if resolution was successfully set
            actual_resolution = (self.capture.get(cv2.CAP_PROP_FRAME_WIDTH), self.capture.get(cv2.CAP_PROP_FRAME_HEIGHT))
            if actual_resolution != (width, height):
                print(f"Warning: Desired resolution {width}x{height} could not be set. Actual resolution is {actual_resolution[0]}x{actual_resolution[1]}")

        # Start the change_resolution function in a separate thread
        threading.Thread(target=change_resolution).start()


if __name__ == '__main__':
    MainApp().run()  # Create an instance of MainApp and run the application
 
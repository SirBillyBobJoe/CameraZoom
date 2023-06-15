import cv2
from datetime import datetime
import os

import threading
from kivy.app import App
from kivy.uix.image import Image
from kivy.graphics.texture import Texture
from kivy.uix.spinner import Spinner
from kivy.uix.floatlayout import FloatLayout
from kivy.core.window import Window
from kivy.uix.scatter import Scatter
from kivy.clock import Clock
from kivy.uix.button import Button
from kivy.uix.relativelayout import RelativeLayout
from kivy.uix.filechooser import FileChooserIconView
from kivy.uix.popup import Popup
from kivy.config import Config
import socket 
from kivy.uix.label import Label


Config.set('input', 'mouse', 'mouse,multitouch_on_demand')  # disable multi-touch


class KivyCamera(Image):
    def __init__(self, capture, fps, **kwargs):
        super(KivyCamera, self).__init__(**kwargs)
        self.capture = capture
        self.fps = fps
        self.event = None
        self.allow_stretch = True  # Allow the image to be resized.
        self.keep_ratio = True # Allow the aspect ratio to be changed.
        self.start()

    def start(self):
        if self.event is None:
            self.event = Clock.schedule_interval(self.update, 1.0 / self.fps)

    def stop(self):
        if self.event is not None:
            self.event.cancel()
            self.event = None

    def update(self, dt):
        ret, frame = self.capture.read()
        if ret:
            # convert it to texture
            buf1 = cv2.flip(frame, 0)
            buf = buf1.tostring()
            image_texture = Texture.create(
                size=(frame.shape[1], frame.shape[0]), colorfmt='bgr')
            image_texture.blit_buffer(buf, colorfmt='bgr', bufferfmt='ubyte')
            # display image from the texture
            self.texture = image_texture
        else:
            print("Error: Unable to read frame from camera")
    @staticmethod
    def get_available_cameras(max_cameras=10):
        cameras = []
        for i in range(max_cameras):
            cap = cv2.VideoCapture(i)
            if cap is None or not cap.isOpened():
                cap.release()
                break
            else:
                cameras.append(i)
                cap.release()
        return cameras
    @staticmethod
    def get_supported_resolutions(cam_id):
        common_resolutions = ['640x480', '1024x768', '1280x720', '1920x1080']
        supported_resolutions = []

        cap = cv2.VideoCapture(cam_id)
        if cap.isOpened():
            for res in common_resolutions:
                width, height = map(int, res.split('x'))
                cap.set(cv2.CAP_PROP_FRAME_WIDTH, width)
                cap.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
                actual_resolution = (cap.get(cv2.CAP_PROP_FRAME_WIDTH), cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
                if actual_resolution == (width, height):
                    supported_resolutions.append(res)
        cap.release()

        return supported_resolutions if supported_resolutions else ['Default']


class CustomScatter(Scatter):
    def on_transform_with_touch(self, touch):
        # Override Scatter's method to prevent it from changing the widget order
        pass

class ScatterImage(Scatter, Image):
    def __init__(self, **kwargs):
        super(ScatterImage, self).__init__(**kwargs)
        self.allow_stretch = True
        self.keep_ratio = True
        self.do_translation = True
        self.do_rotation = False
        self.do_scale = True

    def on_touch_down(self, touch):
        if self.collide_point(*touch.pos):
            return super(ScatterImage, self).on_touch_down(touch)
        return False

    def on_touch_move(self, touch):
        if self.collide_point(*touch.pos):
            return super(ScatterImage, self).on_touch_move(touch)
        return False

    def on_transform_with_touch(self, touch):
        min_scale = max(self.parent.width / self.width, self.parent.height / self.height)

        if self.scale < min_scale:
            self.scale = min_scale
        elif self.scale > 3 * min_scale:  # limiting the max zoom to 3 times the min_scale
            self.scale = 3 * min_scale

        # To ensure that the image never goes out of the screen
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


class MainApp(App):
    def build(self):
        root = FloatLayout()    
        self.capture = cv2.VideoCapture(0)
        self.my_camera = KivyCamera(capture=self.capture, fps=15)
        self.scatter = CustomScatter(do_rotation=False, do_translation=True)  # Use CustomScatter
        self.scatter.size = root.size
        self.scatter.pos = root.pos
        self.scatter.add_widget(self.my_camera)
        
        relative_layout_video = RelativeLayout()
        relative_layout_video.add_widget(self.scatter)

        self.capture_button = Button(text='Capture', size_hint=(None, None), size=(100, 50), pos_hint={'x': 0, 'y': 0})
        self.capture_button.bind(on_release=self.capture_screenshot)
        self.capture_button.opacity = 1  # visible
        self.capture_button.disabled = False  # enabled
        
        relative_layout_button = RelativeLayout()
        relative_layout_button.add_widget(self.capture_button)

        root.add_widget(relative_layout_video)
        root.add_widget(relative_layout_button)
        self.upload_button = Button(text='Upload', size_hint=(None, None), size=(100, 50), pos_hint={'x': 0.2, 'y': 0})
        self.upload_button.bind(on_release=self.open_filechooser)

        relative_layout_button.add_widget(self.upload_button)

        hostname=socket.gethostname()   
        IPAddr=socket.gethostbyname(hostname) 

        #display ip address in top left hand corner of screen in a label
        self.ip_label = Label(text="IP Address: "+IPAddr, size_hint=(None, None), size=(100, 50), pos_hint={'x': 0.045, 'y': 0.94})
        relative_layout_button.add_widget(self.ip_label)


        
        self.spinner = Spinner(
            text='Camera 0',
            values=["Camera "+str(i) for i in KivyCamera.get_available_cameras()],

            size_hint=(None, None),
            size=(100, 50),
            pos_hint={'x': 0.6, 'y': 0})

        self.spinner.bind(text=self.on_spinner_select)
        root.add_widget(self.spinner)
        
        self.resolutions = ['640x480', '1280x720', '1920x1080']
        self.resolution_spinner = Spinner(
            text='Select Resolution',
            values=self.resolutions,
            size_hint=(None, None),
            size=(150, 50),
            pos_hint={'x': 0.75, 'y': 0})

        self.resolution_spinner.bind(text=self.on_resolution_select)
        root.add_widget(self.resolution_spinner)
        self.selected_camera = 0


        

        def update_scatter_size(instance, value):
            self.scatter.size = value
            self.my_camera.size = value

        root.bind(size=update_scatter_size)

        def on_touch_down(window, touch):
            if touch.is_double_tap:
                self.scatter.scale = 1
                self.scatter.pos = root.pos
            return False  # Continue processing the event.

        def on_touch_move(window, touch):
            if len(touch.grab_list) > 0:
                if touch.is_mouse_scrolling:
                    if touch.button == 'scrolldown':
                        if self.scatter.scale > 1:
                            self.scatter.scale = self.scatter.scale * 0.9
                    elif touch.button == 'scrollup':
                        self.scatter.scale = self.scatter.scale * 1.1
                elif touch.grab_current is self.scatter:
                    if len(self.scatter._touches) == 1:  # One finger swipe.
                        touch1 = self.scatter._touches[0]
                        new_x = self.scatter.x + touch1.dx
                        new_y = self.scatter.y + touch1.dy
                        new_x = max(min(new_x, 0), self.scatter.width - self.scatter.width * self.scatter.scale)
                        new_y = max(min(new_y, 0), self.scatter.height - self.scatter.height * self.scatter.scale)
                        self.scatter.pos = (new_x, new_y)
                    elif len(self.scatter._touches) == 2:  # Two finger pinch.
                        # Use distance between the two touches as the scale.
                        touch1, touch2 = self.scatter._touches
                        self.scatter.center = ((touch1.x + touch2.x) / 2, (touch1.y + touch2.y) / 2)  # Center at the midpoint of the two touches.
                        self.scatter.scale = ((touch1.dx**2 + touch1.dy**2)**0.5 +
                                            (touch2.dx**2 + touch2.dy**2)**0.5) / self.scatter.width
                        if self.scatter.scale < 1:
                            self.scatter.scale = 1
            return True  # Return True to indicate that the event was handled.


        def on_transform_with_touch(scatter, touch):
            # Limit the panning to the edge of the image.
            if scatter.x > 0:
                scatter.x = 0
            if scatter.y > 0:
                scatter.y = 0
            if scatter.right < scatter.width:  # use scatter.right which gives the x-coordinate of the right side of the scatter widget
                scatter.right = scatter.width
            if scatter.top < scatter.height:  # use scatter.top which gives the y-coordinate of the top side of the scatter widget
                scatter.top = scatter.height

            # Prevent zooming out past the original size
            if scatter.scale < 1:
                scatter.scale = 1

        Window.bind(on_touch_down=on_touch_down)
        Window.bind(on_touch_move=on_touch_move)
        self.scatter.bind(on_transform_with_touch=on_transform_with_touch)
        
        return root


    def capture_screenshot(self, *args):
        # Hide buttons
        self.capture_button.opacity = 0
        self.upload_button.opacity = 0
        self.spinner.opacity = 0  # make spinner invisible
        self.resolution_spinner.opacity = 0  # make resolution spinner invisible
        self.resolution_spinner.disabled = True  # disable resolution spinner
        
        # Set a delay of 1 second before capturing the screenshot
        Clock.schedule_once(self.capture_screenshot_after_delay, 0.1)

    def capture_screenshot_after_delay(self, *args):
        # Capture screenshot
        ret, frame = self.capture.read()
        if ret:
            timestamp = datetime.now().strftime('%Y%m%d%H%M%S')
            filename = f'screenshot_{timestamp}.png'
            is_written = Window.screenshot(name=filename)
            print(f'Screenshot saved as {filename}') if is_written else print('Error in saving screenshot')

        # Show buttons again
        self.capture_button.opacity = 1
        self.upload_button.opacity = 1
        self.spinner.opacity = 1 # make 
        self.resolution_spinner.opacity = 1  # make resolution spinner visible
        self.resolution_spinner.disabled = False  # enable resolution spinner

    def open_filechooser(self, *args):
        content = FileChooserIconView(path=os.getcwd())
        content.bind(on_submit=self.load_image)
        popup = Popup(title="Select an image", content=content, size_hint=(0.9, 0.9))
        popup.open()

    def load_image(self, instance, value, *args):
        try:
            self.my_camera.stop()
            self.capture.release()
            self.scatter.clear_widgets()  # clear all child widgets
            scatter_image = ScatterImage(source=value[0], size=self.scatter.size, pos=self.scatter.pos)
            self.scatter.add_widget(scatter_image)

            # Hide and disable capture button, show and enable live video button
            self.capture_button.opacity = 0
            self.capture_button.disabled = True
            #disable spinner and resolution button

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
            self.selected_camera = int(text.split()[1])  # update selected camera
            self.scatter.clear_widgets()  # clear all child widgets
            self.my_camera.stop() if self.my_camera else None
            self.capture.release() if self.capture else None
            self.capture = cv2.VideoCapture(self.selected_camera)
            if not self.capture.isOpened():
                print(f"Error: Unable to open camera {self.selected_camera}")
                return
            self.my_camera = KivyCamera(capture=self.capture, fps=15)
            self.my_camera.allow_stretch = True  # Enable stretching the video to fill the screen
            self.my_camera.keep_ratio = True #Enable maintaining the aspect ratio
            self.my_camera.size = self.scatter.size  # Set the size of the camera to match the scatter widget
            self.my_camera.pos = self.scatter.pos  # Set the position of the camera to match the scatter widget
            self.scatter.add_widget(self.my_camera)
            self.my_camera.start()
            self.scatter.scale = 1
            self.scatter.pos = self.root.pos

            # Show and enable capture button, hide and disable live video button
            self.capture_button.opacity = 1
            self.capture_button.disabled = False
            #bring back spinner and resolution button
            self.resolution_spinner.opacity = 1
            self.resolution_spinner.disabled = False
    
    def on_resolution_select(self, spinner, text):
        # Get new resolution parameters
        width, height = map(int, text.split('x'))
        
        # Function to be executed in a separate thread
        def change_resolution():
            # Create a new capture instance with the desired resolution
            new_capture = cv2.VideoCapture(self.selected_camera)
            new_capture.set(cv2.CAP_PROP_FRAME_WIDTH, width)
            new_capture.set(cv2.CAP_PROP_FRAME_HEIGHT, height)
            
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
    MainApp().run()
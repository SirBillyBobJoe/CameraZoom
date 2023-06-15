using Avalonia;
using Avalonia.Controls.ApplicationLifetimes;
using Avalonia.Logging;
using Avalonia.Logging.Serilog;
using Serilog;
using System.Diagnostics;

namespace VideoZoomApp
{
    class Program
    {
        public static void Main(string[] args)
        {
            // Add a listener to output logs to the console
            Trace.Listeners.Add(new ConsoleTraceListener());

            InitializeLogging(); // Initialize Avalonia logging

            LogMessage("Main method executed."); // Added line for troubleshooting

            BuildAvaloniaApp().StartWithClassicDesktopLifetime(args);
        }

        private static void InitializeLogging()
        {
            // Configure Avalonia to use Serilog for logging
            Log.Logger = new LoggerConfiguration()
                .WriteTo.Console()
                .CreateLogger();

            AvaloniaLogger.Sink = new SerilogLoggerSink();
        }

        private static void LogMessage(string message)
        {
            Log.Information(message); // Log the message
        }

        public static AppBuilder BuildAvaloniaApp()
        {
            LogMessage("BuildAvaloniaApp method executed."); // Added line for troubleshooting

            // Rest of the code
        }
    }
}

namespace VideoZoomApp
{
    class Program
    {
        public static void Main(string[] args)
        {
            Console.WriteLine("Main method executed.");
            // Add a listener to output logs to the console
            Trace.Listeners.Add(new ConsoleTraceListener());

            BuildAvaloniaApp().StartWithClassicDesktopLifetime(args);
        }
        private static void InitializeLogging()
                {
                    // Configure Avalonia to use Serilog for logging
                    Log.Logger = new LoggerConfiguration()
                        .WriteTo.Console()
                        .CreateLogger();

                    AvaloniaLogger.Sink = new SerilogLoggerSink();
                }
        // Avalonia configuration, don't remove; also used by visual designer.
        public static AppBuilder BuildAvaloniaApp()
            => AppBuilder.Configure<App>()
                .UsePlatformDetect()
                .LogToTrace(); // Enables logging
            
    }
}

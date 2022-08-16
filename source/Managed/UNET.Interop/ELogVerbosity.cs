namespace UNET.Interop;

[Flags]
public enum ELogVerbosity : byte
{
    /// <summary>
    /// Not used
    /// </summary>
    NoLogging = 0,

    /// <summary>
    /// Always prints a fatal error to console (and log file) and crashes (even if logging is disabled)
    /// </summary>
    Fatal,

    /// <summary>
    /// Prints an error to console (and log file). 
    /// Commandlets and the editor collect and report errors. Error messages result in commandlet failure.
    /// </summary>
    Error,

    ///<summary>
    /// Prints a warning to console (and log file).
    /// Commandlets and the editor collect and report warnings. Warnings can be treated as an error.
    ///</summary> 
    Warning,

    /// <summary>
    /// Prints a message to console (and log file)
    /// </summary>
    Display,

    /// <summary>
    /// Prints a message to a log file (does not print to console)
    /// </summary>
    Log,

    /// <summary>
    /// Prints a verbose message to a log file (if Verbose logging is enabled for the given category, 
    /// usually used for detailed logging) 
    /// </summary>
    Verbose,

    /// <summary>
    /// Prints a verbose message to a log file (if VeryVerbose logging is enabled, 
    /// usually used for detailed logging that would otherwise spam output) 
    /// </summary> 
    VeryVerbose,

    // Log masks and special Enum values

    All = VeryVerbose,
    NumVerbosity,
    VerbosityMask = 0xf,
    SetColor = 0x40, // not actually a verbosity, used to set the color of an output device 
    BreakOnLog = 0x80
};

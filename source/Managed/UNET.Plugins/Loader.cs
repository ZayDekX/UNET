using System.Diagnostics.CodeAnalysis;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using UNET.Exceptions;
using UNET.Interop;

namespace UNET.Plugins;

internal static unsafe class Loader
{
#pragma warning disable IDE0052, CA1823 // Remove unread private members, Avoid unused private fields
    private unsafe struct LoaderDelegates
    {
        private static readonly LoaderDelegates _instance;
        internal static readonly LoaderDelegates* InstancePtr = (LoaderDelegates*)Unsafe.AsPointer(ref _instance);

        public LoaderDelegates()
        { }

        private readonly delegate* unmanaged[Cdecl]<void> _load = &Load;
        private readonly delegate* unmanaged[Cdecl]<void> _unload = &Unload;
        private readonly delegate* unmanaged[Cdecl]<void> _reload = &Reload;
    }
#pragma warning restore IDE0052, CA1823 // Remove unread private members, Avoid unused private fields

    [MemberNotNullWhen(true, nameof(_pluginsPath))]
    private static bool IsInitialized { get; set; }

    private static string? _pluginsPath;

    private static readonly List<Plugin> _plugins = new();

    private static event Action<Assembly>? OnPluginLoaded;

    private static void ReportUnhandledException(object sender, UnhandledExceptionEventArgs e)
    {
        if (e.ExceptionObject is not Exception exception)
        {
            Debug.Log(ELogVerbosity.Error, "Unknown error in managed code");
            return;
        }

        Debug.Log(ELogVerbosity.Error, $"Unhandled managed exception:{Environment.NewLine}\t{exception.Message}");
        Debug.Log(ELogVerbosity.Verbose, exception.StackTrace);
    }

#pragma warning disable CS3016 // Arrays as attribute arguments is not CLS-compliant

    /// <summary>
    /// Initializes UNET <see cref="Core"/>
    /// </summary>
    /// <param name="pluginsPath">Path to directory with plugins</param>
    /// <param name="pathLength">Length of <paramref name="pluginsPath"/></param>
    /// <param name="nativeDelegates">Pointer to external functions, that will be used by Core Core</param>
    /// <param name="loaderDelegates">Pointer to Core Plugin Loader functions that should be exposed</param>
    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    private static void Init(char* pluginsPath, int pathLength, IntPtr nativeDelegates, LoaderDelegates** loaderDelegates)
    {
        if (IsInitialized)
        {
            Debug.Log(ELogVerbosity.Warning, $"Core Plugin Manager is already initialized");
            return;
        }

        _pluginsPath = new string(pluginsPath, 0, pathLength);
        Core.Initialize(nativeDelegates);

        OnPluginLoaded += PluginManager.Initialize;
        AppDomain.CurrentDomain.UnhandledException += ReportUnhandledException;

        *loaderDelegates = LoaderDelegates.InstancePtr;

        Debug.Log(ELogVerbosity.Display, "Managed UNET Core is initialized");

        IsInitialized = true;
    }

    /// <summary>
    /// Loads plugins
    /// </summary>
    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    private static void Load() => LoadPlugins();

    /// <summary>
    /// Unloads plugins
    /// </summary>
    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    private static void Unload() => UnloadPlugins();

    /// <summary>
    /// Reloads plugins
    /// </summary>
    [UnmanagedCallersOnly(CallConvs = new[] { typeof(CallConvCdecl) })]
    private static void Reload() => ReloadPlugins();

    private static void ReloadPlugins()
    {
        UnloadPlugins();
        LoadPlugins();
    }

#pragma warning restore CS3016 // Arrays as attribute arguments is not CLS-compliant

    private static void LoadPlugins()
    {
        if (!IsInitialized)
        {
            throw new NotInitializedException();
        }

        foreach (var path in Directory.EnumerateFiles(_pluginsPath, "*.unetplugin", SearchOption.AllDirectories))
        {
            if (!File.Exists(path))
            {
                throw new FileNotFoundException($"Plugin '{Path.GetFileNameWithoutExtension(path)}' not found");
            }

            LoadPlugin(path);
        }
    }

    private static void LoadPlugin(string path)
    {
        var plugin = new Plugin(path);

        if (!plugin.IsLoaded)
        {
            throw new FileLoadException($"Failed to load plugin from {path}");
        }

        if (!plugin.Assembly.IsDefined(typeof(PluginAttribute)))
        {
            throw new FileLoadException($"'{plugin.Assembly.GetName().Name}' is not a Core plugin");
        }

        _plugins.Add(plugin);

        plugin.Reloaded += OnPluginReloaded;

        OnPluginLoaded?.Invoke(plugin.Assembly);
    }

    private static void OnPluginReloaded(Plugin plugin)
    {
        if (!plugin.IsLoaded)
        {
            throw new InvalidOperationException($"Reloaded plugin is not loaded");
        }

        PluginManager.Refresh(plugin.Assembly);
    }

    private static void UnloadPlugins()
    {
        foreach (var plugin in _plugins)
        {
            plugin.Unload();
        }

        _plugins.Clear();
    }
}
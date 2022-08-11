using System.Diagnostics.CodeAnalysis;
using System.Reflection;
using System.Runtime.Loader;

using McMaster.NETCore.Plugins;

namespace UNET.Plugins;

internal sealed class Plugin
{
    internal Plugin(string path)
    {
        var config = new PluginConfig(path)
        {
            // We can define shared must-have assemblies in UNET.Plugins project references,
            // because DefaultContext is context of this assembly
            PreferSharedTypes = true,

            EnableHotReload = true,
            IsUnloadable = true,
            LoadInMemory = true,
        };

        Loader = new PluginLoader(config);

        Assembly = Loader.LoadDefaultAssembly();
        Context = AssemblyLoadContext.GetLoadContext(Assembly)!;

        Loader.Reloaded += OnReloaded;
        Context.Unloading += OnUnloading;

        _contextReference = new WeakReference(Context);

        IsLoaded = true;
    }

    [MemberNotNullWhen(true, nameof(Context))]
    [MemberNotNullWhen(true, nameof(Assembly))]
    [MemberNotNullWhen(true, nameof(_contextReference))]
    [MemberNotNullWhen(true, nameof(Loader))]
    public bool IsLoaded { get; private set; }

    internal event Action<Plugin>? Reloaded;

    internal event Action<Plugin>? ReloadFailed;

    internal event Action<Plugin>? Unloading;

    internal event Action? Unloaded;

    private WeakReference? _contextReference;

    public AssemblyLoadContext? Context { get; private set; }

    public PluginLoader? Loader { get; }

    public Assembly? Assembly { get; private set; }

    private void OnReloaded(object sender, PluginReloadedEventArgs eventArgs) => Reloaded?.Invoke(this);

    private void OnUnloading(AssemblyLoadContext context) => Unloading?.Invoke(this);

    private void OnUnloaded()
    {
        IsLoaded = false;
        Unloaded?.Invoke();
    }

    internal void Unload()
    {
        if (!IsLoaded)
        {
            return;
        }

        Assembly = null;
        Context.Unload();

        Context = null;

        Loader.Dispose();

        if (_contextReference.IsAlive)
        {
            GC.Collect(GC.MaxGeneration, GCCollectionMode.Forced);
            GC.WaitForPendingFinalizers();
        }

        _contextReference = null;

        OnUnloaded();
    }

    internal void Reload()
    {
        if (!IsLoaded)
        {
            ReloadFailed?.Invoke(this);
            return;
        }

        Loader.Reload();
    }
}
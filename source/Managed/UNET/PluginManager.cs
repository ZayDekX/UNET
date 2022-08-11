using System.Reflection;

using UNET.Exceptions;

namespace UNET;
public sealed class PluginManager
{
    public static void Initialize(Assembly assembly)
    {
        if (!Core.IsInitialized)
        {
            throw new NotInitializedException();
        }

        if (assembly is null)
        {
            throw new ArgumentNullException(nameof(assembly));
        }

        var attribute = assembly.GetCustomAttribute<PluginAttribute>();

        if (attribute is null)
        {
            throw new NotSupportedException($"Assembly {assembly.GetName().Name} is not marked as Plugin");
        }

        var metadata = attribute.MetadataProvider;

        foreach (var classInfo in metadata.Classes)
        {
            Core.NativeDelegates.RegisterManagedClass(classInfo);
        }
    }

    public static void Refresh(Assembly assembly)
    {
        if (!Core.IsInitialized)
        {
            throw new NotInitializedException();
        }

        if (assembly is null)
        {
            throw new ArgumentNullException(nameof(assembly));
        }

        var attribute = assembly.GetCustomAttribute<PluginAttribute>();

        if (attribute is null)
        {
            throw new NotSupportedException($"Assembly {assembly.GetName().Name} is not marked as Plugin");
        }
    }
}

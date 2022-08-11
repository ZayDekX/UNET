namespace UNET;

/// <summary>
/// Used to mark .NET assembly as Core Plugin
/// <para>
/// Simplifies search of plugins and gives easier access to generated metadata
/// </para>
/// </summary>
/// <remarks>
/// Assemblies are marked automatically by source generator, you don't need to implement it by hands
/// </remarks>
[AttributeUsage(AttributeTargets.Assembly, AllowMultiple = false, Inherited = false)]
#pragma warning disable CS3015 // Type has no accessible constructors which use only CLS-compliant types
public abstract class PluginAttribute : Attribute
{
    protected PluginAttribute(IMetadataProvider metadataProvider)
    {
        MetadataProvider = metadataProvider;
    }

    public IMetadataProvider MetadataProvider { get; }
}
#pragma warning restore CS3015 // Type has no accessible constructors which use only CLS-compliant types
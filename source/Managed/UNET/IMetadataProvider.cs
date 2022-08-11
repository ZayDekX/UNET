namespace UNET;

public interface IMetadataProvider
{
    public IEnumerable<nint> Classes { get; }
}

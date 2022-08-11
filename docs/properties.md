# Properties

You can access real values stored in UE objects only via pointers. UNET will manage those pointers for you and provide an easy-to-use API:
- `ReadOnlyUProperty<T>` allows to read values, but not write
- `UProperty<T>` allows to read and write values

You can use attributes to change generated metadata the same way as for class: `PropertyInfo` and `Meta`.

To define exposed property, just create a common init-only C# property:
```csharp
[UClass("Object")]
public class MyObject
{
    // this property will be exposed to Unreal Engine
    public UProperty<int> MyExposedInteger { get; init; }

    // this property won't be exposed to Unreal Engine
    public int MyHiddenInteger { get; init; }
}
```

UNET will search for exposed properties in exposed types and generate required metadata.  

Exposed or existing properties have some limitations:
- They don't support DI, because injected in other way.
- They must be init-only.
- Their type must be wrapped with `UProperty<T>` or `ReadOnlyUProperty<T>`.
- They can't be defined in hidden types.

## Property lifetime

Property has the same lifetime as facade it belongs to.
# Class metadata

> **Note**: This page contains information about design part of features (how they look like in code).  
> For implementation details see [Metadata generation] page.

## Summary

Access to Unreal Engine objects (instances of `UObject` and it's derivatives) from .NET can be made only via using wrappers on .NET side, so:
- All exposed .NET classes must have some metadata, which will be used by Unreal Engine to register those exposed .NET classes.
- Unreal Engine requires lots of metadata to register any class. 
This metadata is a boilerplate code, which is very hard to maintain by hands, so it have to be generated on compile-time.
- Exposed types can be detected by attributes, so all exposed .NET classes must have some specific attributes attached to them. 
Those attributes can be used to change generated metadata.

## Naming

According to [UE Naming Conventions], in C++ code type names **are prefixed with** an additional **upper-case letter** to distinguish them from variable names:
- U ─ Inherited from `UObject`
- A ─ Inherited from `AActor`
- S ─ Inherited from `SWidget`
- T ─ Template classes
- I ─ Interfaces
- E ─ Enums
- F ─ Structs

Because **all types** known by Unreal Engine **must have a prefix**, exposed types will have it, but **only in metadata**.  

> **Note**: Source generator will add prefix for exposed types, so there is no need to do it manually!

## Class

It's impossible to use native type as a real parent for managed class, but it is possible to create a partial wrapper for existing native type, because Unreal Engine have a reflection system, which allows to receive all the required information about requested type.

> **Note**: Information about native types can be obtained only via Unreal Engine Reflection system, which means that only types marked with `UCLASS()`, `USTRUCT()`, `UENUM()` or `UINTERFACE()` can be obtained.

The most easy and simple solution is to provide an attribute similar to UE macro `UCLASS()` which will be used in the same way:
- `UClass` attribute can be used to create new UE class and bind .NET wrapper to them
- `NativeUClass` can be used to create and bind .NET wrapper to existing UE class
- `ClassInfo` attribute can be used to provide additional information about exposed or imported classes
- `Meta` attribute can be used to provide additional editor-only metadata

This attributes will be used by source generator to create all the required metadata.

> **Note**: `ClassInfo` and `Meta` attributes will have effect only when `UClass` or `NativeUClass` attributes are specified

### Style

C++ and C# code will look and feel almost the same, but a little bit cleaner and more readable.

*C++ version*:
```cpp
UCLASS()
class UMyAwesomeNativeObject : public UObject
{
    GENERATED_BODY()
    
    public:

    UPROPERTY()
    int SpecialInteger;

    // implementation
}
```

*C# version*:
```csharp
[UClass("Object")]
public class MyAwesomeManagedObject
{
    public UProperty<int> SpecialInteger { get; init; }

    // implementation
}
```

### [Examples](#class-1)

## Property

There are several ways to implement interaction with real data, but the simplest one ─ access real value via pointer.  
Because it is not possible to intercept pointers on source generation phase of compilation, as a solution will be special types, which will allow to interact with UE objects and values:
- `UProperty<T>` allows to read and write values
- `ReadOnlyUProperty<T>` allows only reading of values

Also there are some attributes, that allow to change metadata: `PropertyInfo` and `Meta`.

All instances of property wrappers are created when class wrapper instance is being created. For more info read [Object handling].

### [Examples](#property-1)

## Function

> Not implemented yet.

## Event (Delegate)

> Not implemented yet.

---

# Possible scenarios and use cases

## Class

### Replicate existing type

Cases below illustrate how existing types can be imported and replicated

---

**Case**: I want to create wrapper for already existing UE type  
**Solution**:

> *ExistingType.cs*:
> ```csharp
> [NativeUClass]
> public class ExistingType
> {
>     // implementation
> }
> ```
>
> `NativeUClass` attribute is used to create a replica of existing Unreal Engine type. 
> Name of type must be the same as type in UE, but without prefix, otherwise `TypeNotFoundException` may be thrown

---

**Case**: I want to inherit new wrapper from imported wrapper  
**Solution**:

> *ImportedType.cs*:
> ```csharp
> [NativeUClass]
> public class ImportedType
> {
>    // implementation
> }
> ```
>
> *ChildType.cs*:
> ```csharp
> [UClass]
> public class ChildType : ImportedType
> {
>     // implementation
> }
> ```
>
> As in previous case, `ImportedType` should have a `NativeUClass` attribute, but `ChildType` also must have `UClass` attribute, which is required to expose this type.

---

### Create new type

Cases below illustrate how new types can be created and exposed to Unreal Engine

---

**Case**: I want to expose my .NET class to Unreal Engine  
**Solution**:

>  *ExposedType.cs*:
>  ```csharp
>  [UClass("NativeParent")]
>  public class ExposedType
>  {
>      // implementation
>  }
>  ```
>  
>  To expose C# class, just mark it with `UClass` attribute. 
>  Source generator will produce all the required metadata according to provided attribute arguments.

---

**Case**: I want to inherit my exposed type from .NET type or implement interfaces  
**Solution**:
> *ExposedType.cs*
> ```csharp
> [UClass("NativeParent")]
> public class ExposedType : ManagedParent, IDotNetInterface
> {
>     public ExposedType() { }
> 
>     // implementation
> }
> ```
>
> It is possible, but public parameterless constructor is required. 
> Child of a managed class just extends data and functionality of it's parent, which is fully available on .NET side. 
> But Unreal Engine will know only about types that were provided.
>
> **Warning**: information about managed parent will not be exposed, unless it is marked with `UClass` attribute.

---

**Case**: I want to inherit my exposed .NET type from another exposed .NET type  
**Solution**:

> *ExposedType.cs*:
> ```csharp
> [UClass]
> public class ExposedType : ExposedManagedParent
> {
>     public ExposedType() { }
> 
>     // implementation
> }
> ```
>
> It is possible, because source generator **does not** extend your type. 
> Instead it generates a new type inside of separate assembly, which contains all the metadata about your type. Also name of a managed parent is required to be unspecified in attribute, because it is already specified by inheritance.

**Case**: I want to use DI to inject my service to exposed type  
**Solution**:

> *ExposedType.cs*:
> ```csharp
> [UClass("NativeParent")]
> public class ExposedType
> {
>     private readonly IRequiredService _requiredService;
>     private readonly IOptionalService _optionalService;
>
>     public ExposedType
>     (
>         [InjectRequired] IRequiredService requiredService, 
>         [InjectOptional] IOptionalService optionalService
>     ) 
>     {
>         _requiredService = requiredService;
>         _optionalService = optionalService;
>     }
> 
>     [InjectRequired]
>     IRequiredService AnotherRequiredService { get; init; }
>
>     [InjectOptional]
>     IOptionalService AnotherOptionalService { get; init; }
>
>     // implementation
> }
> ```
>
> UNET supports both property and constructor DI. 
> Also it is possible to manually resolve targets by requesting `IServiceProvider` in constructor.  
> For more info read [Object handling].
---

**Case**: I want to make hidden (internal) implementation of exposed type, which will be unknown to Unreal Engine  
**Solution**:

> *ExposedManagedParent.cs*
> ```csharp
> [UClass("NativeParent")]
> [TargetResolver(typeof(HiddenWrapperResolver))]
> public class ExposedManagedParent
> {
>     public ExposedType() { }
> 
>     // implementation
> }
> ```
>
> *InternalTypeImplementation.cs*:
> ```csharp
> internal class InternalTypeImplementation : ExposedManagedParent
> {
>     public ExposedType() : base() { }
> 
>     // implementation
> }
> ```
>
> *HiddenWrapperResolver.cs*:
> ```csharp
> public class HiddenWrapperResolver : ITypeResolver
> {
>     public Type Resolve()
>     {
>          return typeof(InternalTypeImplementation);
>     }
> }
> ```
>
> It is possible, but custom type resolver is required to produce hidden implementations of wrappers. Type of resolver must be specified with `TargetResolver` attribute in exposed type.  
>
> For more info read [Object handling](object-handling.md#creating-instance-of-hidden-implementation-of-exposed-wrapper). 

---

## Property

**Case**: I want to define property in exposed or imported type that can access native data 
**Solution**:

> *SomeType.cs*:
> ```csharp
> [UClass("Object")]
> public class SomeType
> {
>     [PropertyInfo(BlueprintReadable = true)]
>     public UProperty<int> SomeInteger { get; init; }
> }
> ```
> Just wrap target type with `UProperty<>`.
>
> **Note**: `PropertyInfo` attribute can be used to change property metadata and can be ommited.

---

**Case**: I want to define a read-only property in exposed or imported type that can access native data  
**Solution**:

> *SomeType.cs*:
> ```csharp
> [UClass("Object")]
> public class SomeType
> {
>     public ReadOnlyUProperty<int> SomeInteger { get; init; }
> }
> ```
> Use `ReadOnlyUProperty` as wrapper instead of `UProperty`

---

**Case**: I want to create UProperty in a type that is not interacting with Unreal Engine  
**Solution**:

> It is possible, but not recommended, because it can lead to unexpected behavior.

---

[Object handling]: object-handling.md
[Metadata generation]: metadata-generation.md
[UE Naming conventions]: https://docs.unrealengine.com/4.27/en-US/ProductionPipelines/DevelopmentSetup/CodingStandard/#namingconventions
[.NET GC Fundamentals]: https://docs.microsoft.com/en-us/dotnet/standard/garbage-collection/fundamentals#memory-release
# Class metadata

> **Note**: This page contains information about design part of features (how they look like in code).  
> For implementation details see [Metadata generation](metadata-generation.md) page.

## Summary

Access to Unreal Engine objects (instances of `UObject` and it's derivatives) from .NET can be made only via using wrappers on .NET side, so:
- All exposed .NET classes must have some metadata, which will be used by Unreal Engine to register those exposed .NET classes.
- Unreal Engine requires lots of metadata to register any class. 
This metadata is a boilerplate code, which is very hard to maintain by hands, so it have to be generated on compile-time.
- Exposed types can be detected by attributes, so all exposed .NET classes must have some specific attributes attached to them. 
Those attributes will be used to change generated metadata.

## Wrapper instance lifetime

Garbage Collector in .NET can destroy object when nothing is referencing that object.  
If reference to an instance of C# struct is passed to unmanaged code, GC won't know about that reference and can delete or move this object.  

According to [.NET GC Fundamentals](https://docs.microsoft.com/en-us/dotnet/standard/garbage-collection/fundamentals#memory-release) static fields are application roots.  
It means that objects referenced by static field can't be collected, which leads to use of static object that will contain all instances of all wrappers by some unqiue ID. 

Wrapper can be instantiated only after UE object it belongs to. Also lifetime of wrapper is limited to lifetime of owning object.

> **Warning**: do not reference wrappers from other objects! It can lead to unexpected behavior, in most cases to memory leaks, `AccessViolationException` and `NullReferenceException` or even silent data corruption which can be very hard to debug.

## Naming

According to [Naming Conventions](https://docs.unrealengine.com/4.27/en-US/ProductionPipelines/DevelopmentSetup/CodingStandard/#namingconventions) of UE Coding Standard, in C++ code type names **are prefixed with** an additional **upper-case letter** to distinguish them from variable names:
- U ─ Inherited from `UObject`
- A ─ Inherited from `AActor`
- S ─ Inherited from `SWidget`
- T ─ Template classes
- I ─ Interfaces
- E ─ Enums
- F ─ Structs

Because **all types** known by Unreal Engine **must have a prefix**, exposed types will have it, but **only in metadata**.  

> **Note**: Source generator will add prefix for exposed types, so there is no need to do it manually!

# Details

This section describes some use case scenarios and solutions divided by categories.

## Class

It's impossible to use native type as a real parent for managed class, but it is possible to create a partial wrapper for existing native type, because Unreal Engine have a reflection system, which allows to receive all the required information about requested type.

> **Note**: Information about native types can be obtained only via Unreal Engine Reflection system, which means that only types marked with `UCLASS()`, `USTRUCT()`, `UENUM()` or `UINTERFACE()` can be obtained.

The most easy and simple solution is to provide an attribute similar to UE macro `UCLASS()` which will be used in the same way:
- `UClass` attribute can be used to create new UE class and bind .NET wrapper to them.
- `NativeUClass` can be used to create and bind .NET wrapper to existing UE class.
- `ClassInfo` attribute can be used to provide additional information about exposed or imported classes.

This attributes will be used by source generator to create all the required metadata.

> ## Scenarios

---

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
>  To expose C# class, just mark it with `[UClass()]` attribute. 
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
> **Warning**: information about managed parent will not be exposed, unless it is marked with `UClassAttribute`.

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

---

**Case**: I want to inherit my type from already exposed type, but without exposing new one  
**Solution**:

> Not implemented.

---

## Property

> Not implemented yet.

> ## Scenarios
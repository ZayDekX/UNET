# Classes

You can't use native type as parent of managed class or vice versa, but you can create a facade for that type.

UNET provides special attributes that are similar to UE macros and work in almost the same way:
- `UClass` allows to create facade for new or already existing type
- `ClassInfo` allows to provide additional metadata, that can't be received from usage
- `Meta` allows to set editor-only metadata

> **Note**: `ClassInfo` and `Meta` attributes will have effect only when `UClass` attribute is specified.

As you can see in next examples, C# code will look and feel almost the same as C++, but cleaner and more readable:

*C++*
```cpp
UCLASS()
class UMyAwesomeNativeObject : public UObject
{
    GENERATED_BODY()
    
    public:

    UPROPERTY()
    int SpecialInteger;

    UFUNCTION()
    float Calculate(float a, float b)
    {
        return a + b;
    }

    // implementation
}
```

*C#*
```csharp
[UClass("Object")]
public class MyAwesomeManagedObject
{
    public UProperty<int> SpecialInteger { get; init; }

    [UFunction]
    public float Calculate(float a, float b)
    {
        return a + b;
    }

    // implementation
}
```

## Facades

With UNET you can create different kinds of facades:

### For external types

Just create `public abstract class` and mark it with `UClass` attribute like in example below:  

```csharp
[UClass]
public abstract class NativeType
{
    // implementation
}
```

### For exposed types

Just create `public non-abstract* non-static class` marked with `UClass` attribute

*\*Unless you provide TypeResolver. Read [object handling] for more info.*

You can inherit your exposed type by extending already existing one:

```csharp
[UClass]
public class ManagedType : NativeType
{
    // implementation
}
```

Or by specifying name of external parent as argument of `UClass` attribute:
```csharp
[UClass("NativeType")]
public class ManagedType
{
    // implementation
}
```

> **Note**: UE `Object` will be used as default if you won't specify name of parent type.

### For hidden types

Just extend existing facade that has a `TypeResolver`:
```csharp
public class HiddenType : ManagedType
{
    // implementation
}

[UClass]
[TypeResolver(typeof(ManagedTypeTypeResolver))]
public abstract class ManagedType : NativeType
{
    // implementation
}

public class ManagedTypeTypeResolver : ITypeResolver
{
    public Type Resolve()
    {
        return typeof(HiddenType);
    }
}
```

> **Note**: TypeResolver supports Dependency Injection and can have more complex logic.

## Naming

According to [UE Naming Conventions], C++ code type names are prefixed with an additional upper-case letter to distinguish them from variable names:
- U ─ Inherited from `UObject`
- A ─ Inherited from `AActor`
- S ─ Inherited from `SWidget`
- T ─ Template classes
- I ─ Interfaces
- E ─ Enums
- F ─ Structs

Types known to UE Reflection System must have a prefix.  
It means that exposed types also will have a prefix, but only in metadata.

> **Note**: You don't need to add prefix by hands ─ source generator will automatically add it in metadata.

## Inheritance and instancing

UE types can inherit from each other. UNET facades also allow support of inheritance to better replicate UE type hierarchy.  
Object handling creates some limitations and a couple of cases for instancing, which are illustrated in the next table:

| Child    |         | Parent   |         |
|----------|---------|----------|---------|
|          | Exposed | External | Hidden  |
| Exposed  | Auto    | Auto     | Auto    |
| External |         |          |         |
| Hidden   | Manual* |          | Manual* |

*\*Only if last exposed parent has `TypeResolver`*

In other words:
- Exposed facades will always have instances.  
- External facades won't have instances (behavior of abstract class).  
- Hidden facades can have instances only if their exposed parent have a type resolver.

> **Note**: You can create hidden facade by extending one for external type, but to create it's instance you need to create exposed type.

When exposed type is instantiated by Unreal Engine, UNET will create instance of facade via generated method `CreateInstance` in metadata type.  

### Instance of exposed type

In scenarios when type of facade is known both in runtime and compile-time, source generator will implement `CreateInstance` method as straight call to constructor of that type.  

*Example of generated `CreateInstance` method:*
```csharp
public static IObject CreateInstance
(
    nint nativePointer, 
    IServiceProvider serviceProvider
)
{
    return new ExposedType
    (
        serviceProvider.GetRequiredService<IRequiredService>()
    )
    {
        A = new UProperty<int>(nativePointer + A_PropertyInfo.Offset),
        B = new UProperty<float>(nativePointer + B_PropertyInfoOffset),
        OptionalService = serviceProvider.GetService<IOptionalService>()
    };
}
```

> **Note**: C# 9.0 allows to omit type in `new expression`, which is used by UNET source generator.

### Instance of hidden type

In scenarios when target type is unknown for both runtime and compile-time, there is no way to create instance.

To solve that issue, as mentioned in [facades section](#for-hidden-types) of this document, hidden implementations require custom type resolver to create their instances.

Writing a type resolver is very simple task ─ just create class, which implements `ITypeResolver` interface as shown in example below:

```csharp
public class ExampleResolver : ITypeResolver
{
    public ExampleResolver([InjectRequired] IRequiredService requiredService)
    {
        // implementation of constructor
    }

    public Type Resolve()
    {
        // there can be more complex logic
        return typeof(InternallyImplementedClass);
    }
}
```

This resolver will be automatically constructed when UNET plugin is loading. Also type resolvers support DI via both constructor and properties. 

As mentioned in previous section ─ by default UNET source generator will produce `CreateInstance` method with a straight call to constructor of a facade.  
In that scenario source generator will produce a small factory, which will get type from user-defined type resolver:

```csharp
public static IObject CreateInstance(nint nativePointer, IServiceProvider serviceProvider)
{
    var resolverProvider = serviceProvider.GetRequiredService<ITypeResolverProvider>();
    var typeResolver = resolverProvider.GetResolver<BaseExposedType>();
    var resolvedType = typeResolver.Resolve();

    return resolvedType.Name switch {
        nameof(InternallyImplementedClass) => 
        new InternallyImplementedClass
        (
            serviceProvider.GetRequiredService<IRequiredService>()
        )
        {
            A = new UProperty<int>(nativePointer + A_PropertyInfo.Offset),
            B = new UProperty<float>(nativePointer + B_PropertyInfo.Offset),
            OptionalService = serviceProvider.GetService<IOptionalService>()
        },
        nameof(AnotherImplementation) =>
        new AnotherImplementation
        (
            serviceProvider.GetRequiredService<IRequiredService>()
        )
        {
            A = new UProperty<int>(nativePointer + A_PropertyInfo.Offset),
            B = new UProperty<float>(nativePointer + B_PropertyInfo.Offset),
            AnotherOptionalService = serviceProvider.GetService<IOptionalService>(),
            AnotherRequiredService = serviceProvider.GerRequiredService<IRequiredService>()
        },
        _ => throw new ArgumentOutOfRangeException()
    }
}
```

> **Note**: facades for existing UE types like `Actor`, `Object` or `Widget` can't be instantiated, because those types are already implemented in C++ and their construction process can't be changed without creating a child type.  
> Also it disallows abuse of facades to change behavior of existing type.

### Facade instance lifetime

Garbage Collector in .NET can destroy object when nothing is referencing that object.  
If reference to an instance of C# struct is passed to unmanaged code, GC won't know about that reference and can delete or move this object.  

According to [.NET GC Fundamentals], static fields are application roots.  
It means that objects referenced by static field can't be collected, which allows to use static collection as container for instances of facades, which can be accessed by some unqiue ID. 

Facade can be instantiated only after UE object it belongs to. Also lifetime of facade is limited to lifetime of that object.

> **Warning**: do not reference facades from other objects! It can lead to unexpected behavior, in most cases to memory leaks, `AccessViolationException` and `NullReferenceException` or even silent data corruption which can be very hard to debug.

### Dependency Injection

UNET allows you to use Dependency Injection via both constructors and properties. You just need to mark injection point with `InjectRequired` or `InjectOptional` attribute.

UNET allows to use constructors only for dependency injection.
If you won't specify attribute for your service, UNET will try to inject it like it was marked with `InjectRequired`.

This feature comes from automatic nature of facade instancing and possible need of dependency injection in a fast and reliable way.

```csharp
[UClass]
public class MyExposedClass
{
    private readonly IRequiredService _requiredService;
    private readonly IOptionalService _optionalService;

    public MyExposedClass
    (
        [InjectRequired] IRequiredService requiredService,
        [InjectOptional] IOptionalService optionalService
    )
    {
        _optionalService = optionalService;
        _requiredService = requiredService;
    }

    [InjectRequired]
    public IRequiredService RequiredService { get; init; }

    [InjectOptional]
    public IOptionalService OptionalService { get; init; }
}
```

> **Note**: you can also inject `IServiceProvider` and manually inject your services.

[UE Naming conventions]: https://docs.unrealengine.com/4.27/en-US/ProductionPipelines/DevelopmentSetup/CodingStandard/#namingconventions
[.NET GC Fundamentals]: https://docs.microsoft.com/en-us/dotnet/standard/garbage-collection/fundamentals#memory-release

[C++/CLI]: https://docs.microsoft.com/en-us/cpp/dotnet/dotnet-programming-with-cpp-cli-visual-cpp
[Function Pointers]: https://docs.microsoft.com/en-us/dotnet/csharp/language-reference/proposals/csharp-9.0/function-pointers
[Language independence]: https://docs.microsoft.com/en-us/dotnet/standard/language-independence

[C# Source Generators]: https://docs.microsoft.com/en-us/dotnet/csharp/roslyn-sdk/source-generators-overview
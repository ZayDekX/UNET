# Object handling

UNET allows to create two types of wrappers:
1) For existing types  
    UNET **will not** create instances of wrappers
2) For new types  
    UNET **will** create instance of wrapper for bound object.

This behavior allows to simplify implementation, and disallows overrides of existing behavior defined by already existing external implementation.

Sometimes projects by design require Dependency Injection. Trying to implement DI in solutions, where objects created automatically, will require to use reflection and bad code practices, which leads to overcomplicated code and bad performance. 

To avoid that UNET is using DI by default and automatically generates initializers for required services.

> **Note**: UNET allows to use DI via both properties and constructors by using default .NET implementation.

## Creating instance of exposed wrapper

UNET is a module for Unreal Engine, which defines a special type of classes by extending native `UClass` type. It allows to handle UE object instances in special way ─ production of .NET wrappers for UE objects.  
Metadata type of a wrapper contains all the necessary information and methods, which allow to create instance of wrapper. 

When exposed type is instantiated by Unreal Engine, UNET will create instance of wrapper via **generated** method `CreateInstance` in metadata type.  

*Example of generated `CreateInstance` method:*
```csharp
public static IObject CreateInstance(nint nativePointer, IServiceProvider serviceProvider){
    return new ExposedType(requiredService: serviceProvider.GetRequiredService<IRequiredService>())
    {
        A = new UProperty<int>(nativePointer + A_PropertyInfo.Offset),
        B = new UProperty<float>(nativePointer + B_PropertyInfo.Offset),
        OptionalService = serviceProvider.GetService<IOptionalService>()
    };
}
```

> **Note**: C# 9.0 allows to omit type in new expression, which is used by UNET source generator.

## Creating instance of hidden implementation of exposed wrapper

In scenarios when type of wrapper is known both in runtime and compile-time, source generator will implement `CreateInstance` method as straight call to constructor of that type.  
But in that specific scenario target type is unknown for both runtime and compile-time. It means, that there is no way to create instance.

To solve that issue, as mentioned in [Class metadata] document, hidden implementations require custom type resolver to create their instances.  
Writing a type resolver is very simple ─ just create class, which implements `ITypeResolver` interface as shown in example below:

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

This resolver will be automatically constructed when UNET plugin is loaded. Also type resolvers support DI via both constructor and properties. 

As mentioned above ─ by default UNET source generator will produce `CreateInstance` method with a straight call to constructor of a wrapper.  
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
            requiredService: serviceProvider.GetRequiredService<IRequiredService>()
        )
        {
            A = new UProperty<int>(nativePointer + A_PropertyInfo.Offset),
            B = new UProperty<float>(nativePointer + B_PropertyInfo.Offset),
            OptionalService = serviceProvider.GetService<IOptionalService>()
        },
        nameof(AnotherImplementation) =>
        new AnotherImplementation
        (
            requiredService: serviceProvider.GetRequiredService<IRequiredService>()
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

> **Note**: Wrappers for existing UE types like `Actor`, `Object` or `Widget` can't be instantiated, because those types are already implemented in C++ and their construction process can't be changed without creating a child type.

## Wrapper instance lifetime

Garbage Collector in .NET can destroy object when nothing is referencing that object.  
If reference to an instance of C# struct is passed to unmanaged code, GC won't know about that reference and can delete or move this object.  

According to [.NET GC Fundamentals], static fields are application roots.  
It means that objects referenced by static field can't be collected, which allows to use static collection as container for instances of wrappers, which can be accessed by some unqiue ID. 

Wrapper can be instantiated only after UE object it belongs to. Also lifetime of wrapper is limited to lifetime of that object.

> **Warning**: do not reference wrappers from other objects! It can lead to unexpected behavior, in most cases to memory leaks, `AccessViolationException` and `NullReferenceException` or even silent data corruption which can be very hard to debug.

[Class metadata]: class-metadata.md
[.NET GC Fundamentals]: https://docs.microsoft.com/en-us/dotnet/standard/garbage-collection/fundamentals#memory-release
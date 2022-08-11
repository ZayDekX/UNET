# Installation

## Download

Just clone this repository to any desired location on your machine.

## .NET Runtime

UNET is natively hosting CLR to run your managed code in Unreal Engine.  
It means, that .NET Runtime binaries are required to use UNET.  
You can download them [here](https://dotnet.microsoft.com/en-us/download/dotnet) or build from sources by yourself.

> **Note**: UNET requires .NET 6.0 or later.

After downloading, place binaries at `<UNET repo>/Binaries/Runtime` directory.

## Add UNET to UE project

UNET is a plugin for Unreal Engine, which needs to be installed for each project separately.  
There are several ways to add UNET to your UE project. 

> **Note**: You can install UNET only to existing projects. If you don't have UE project, just create it.

UNET can be installed in several ways, that are described in next sections.

### Automatic installation

Run next CLI command in `<UNET repo>/Installer` directory:
```
dotnet run -- <Your UE Project Path> --install 
```

> **Note**: Installer will warn you, if you haven't provided .NET libraries.

Installer will copy contents of `<UNET repo>/Source/Native` directory to `<Your UE Project>/Plugins/UNET` directory 
and your .NET Runtime binaries to `<Your UE Project>/Plugins/UNET/Binaries/Runtime` directory.  
Also it will automatically build `UNET.Plugins` project and place binaries in `<Your UE Project>/Plugins/UNET/Binaries/Managed` directory.

After installation is done, restart your UE Editor to compile native UNET module and enable it in your project.

### Manual

1. Copy contents of `<UNET repo>/Source/Native` to `<Your UE Project>/Plugins/UNET` directory.
2. Build `UNET.Plugins` project from source and place binaries to `<Your UE Project>/Plugins/UNET/Binaries/Managed` directory.
3. Copy your .NET Runtime binaries to `<Your UE Project>/Plugins/UNET/Binaries/Runtime` and restart UE Editor.

## Creating UNET projects

### Automatic

Run next CLI command in `<UNET repo>/Installer` directory:
```
dotnet run -- <Your UE Project Path> --project <Your C# Project Name>
```

Installer will create ready-to-work project with specified name in `<Your UE Project Path>/ManagedSource/<Your C# Project name>` directory.

### Manual

Create new C# project in `<Your UE Project Path>/ManagedSource/<Your C# Project Name>` directory and change your `.csproj` file to enable support of UNET:
```xml
<Project Sdk="Microsoft.NET.Sdk">
    <PropertyGroup>
        <TargetFramework>net6.0</TargetFramework>
        <LangVersion>latest</LangVersion>
        <Platforms>x64</Platforms>
        
        <Nullable>enable</Nullable>  
        <ImplicitUsings>enable</ImplicitUsings>  
        
        <OutputPath>../../Binaries/Managed</OutputPath>
        <AppendTargetFrameworkToOutputPath>False</AppendTargetFrameworkToOutputPath>
        <AppendRuntimeIdentifierToOutputPath>False</AppendRuntimeIdentifierToOutputPath>
    </PropertyGroup>

    <ItemGroup>
        <PackageReference Include="UNET" />
        <PackageReference Include="UNET.Generators" 
                          OutputItemType="Analyzer"
                          ReferenceOutputAssembly="false"/>
    </ItemGroup>
</Project>
```

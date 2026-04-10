# gRPC Migration Notes

This directory contains the first protobuf contract that replaces the legacy
ZeroC Ice interfaces from:

- `CC-Src/CC-Client/src/Slice/IController.ice`
- `CC-Src/CC-Client/src/Slice/IStationStateReceiver.ice`
- `CC-Src/CC-Client/src/Slice/AppControlParameter.ice`
- `CC-Src/CC-Client/src/Network.ice`
- `CC-Src/CC-Client/src/Version.ice`

## Transport choice

- Rust client/backend: `tonic`
- .NET station service: `grpc-dotnet`
- Shared contract: `src-tauri/proto/cc.proto`

## Service split

- `StationControl`
  - unary control and query methods
- `FileTransfer`
  - client-streaming upload
  - server-streaming download
- `Telemetry`
  - bidirectional stream replacing the old Ice callback receiver pattern

## Rust code generation

`src-tauri/build.rs` now compiles `proto/cc.proto` with `tonic-prost-build`.
`protoc` is vendored through `protoc-bin-vendored`, so local system packages are
not required just to build the Rust side.

Generated types are exposed through:

- `src-tauri/src/grpc.rs`

## Minimal .NET hosting shape

`grpc-dotnet` uses normal ASP.NET Core service registration:

```csharp
var builder = WebApplication.CreateBuilder(args);
builder.Services.AddGrpc();

var app = builder.Build();
app.MapGrpcService<StationControlService>();
app.MapGrpcService<FileTransferService>();
app.MapGrpcService<TelemetryService>();
app.Run();
```

Typical `.csproj` protobuf item setup:

```xml
<ItemGroup>
  <PackageReference Include="Grpc.AspNetCore" Version="*" />
  <PackageReference Include="Grpc.Tools" Version="*" PrivateAssets="All" />
  <PackageReference Include="Google.Protobuf" Version="*" />
</ItemGroup>

<ItemGroup>
  <Protobuf Include="Protos\\cc.proto" GrpcServices="Server" />
</ItemGroup>
```

## Current implementation status

`CC-rClient` now has a real tonic-based control adapter for the completed
`CC-rStationService` RPCs that cover:

- `start app`
- `restart app`
- `exit app`
- `shutdown`
- `reboot`
- display/page commands
- telemetry-backed runtime refresh
- screen capture
- remote file browse / upload / download / rename

The remaining migration work is centered on:

- richer parity with the legacy Qt station-status views and specialized dialogs

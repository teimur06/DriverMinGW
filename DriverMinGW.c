
    #include <ntddk.h>

#include "names.h" 

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
VOID UnloadRoutine(IN PDRIVER_OBJECT DriverObject);

UNICODE_STRING registryPath;
PDEVICE_OBJECT fdo;
UNICODE_STRING dosDevicesName;
UNICODE_STRING deviceName;

/*
static NTSTATUS AddDevice(PDRIVER_OBJECT DriverObject, PDEVICE_OBJECT pdo)
{
    DbgPrint("AddDevice\n");
    return STATUS_SUCCESS;
}*/
 
static NTSTATUS DispatchCreate( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp )
{
    DbgPrint("my_dispatch_create\n");
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest( Irp, IO_NO_INCREMENT );
    
    return STATUS_SUCCESS;
}

static NTSTATUS DispatchDeviceControle( IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp)
{
    PIO_STACK_LOCATION io_stack;
    DATA_IRP * data_irp;
    ULONG controlCode = 0;
    PVOID SystemBuffer = NULL;
    ULONG inputBufferLength = 0;
    ULONG outputBufferLength = 0;
    
    DbgPrint("Start DispatchDeviceControle\n");
    
    Irp->IoStatus.Status = STATUS_NOT_IMPLEMENTED;
    Irp->IoStatus.Information = 0;
    
    io_stack = IoGetCurrentIrpStackLocation(Irp);
    
    if (!io_stack)
    {
        DbgPrint("IoGetCurrentIrpStackLocation error\n");
        Irp->IoStatus.Status = STATUS_INTERNAL_ERROR;
        return Irp->IoStatus.Status;
    }
    
    if (io_stack->MajorFunction != IRP_MJ_DEVICE_CONTROL)
    {
        DbgPrint("io_stack->MajorFunction != IRP_MJ_DEVICE_CONTROL\n");
        Irp->IoStatus.Status = STATUS_INTERNAL_ERROR;
        return Irp->IoStatus.Status;
    }
    
    controlCode = io_stack->Parameters.DeviceIoControl.IoControlCode;
    SystemBuffer = Irp->AssociatedIrp.SystemBuffer;
    inputBufferLength = io_stack->Parameters.DeviceIoControl.InputBufferLength;
    outputBufferLength = io_stack->Parameters.DeviceIoControl.OutputBufferLength;
    
    switch (controlCode)
    {
        case IOCTL_TEST:
            data_irp = (DATA_IRP *) SystemBuffer;
            if ((inputBufferLength < sizeof (DATA_IRP)) || (!data_irp) )
            {
                DbgPrint("io_stack->MajorFunction != IRP_MJ_DEVICE_CONTROL\n");
                Irp->IoStatus.Status = STATUS_INVALID_PARAMETER;
                break;
            }         
            
            DbgPrint("ioctl(1) called, arg = %d\r\n", data_irp->code );
            DbgPrint("inputBufferLength = %d\r\n", inputBufferLength );
            DbgPrint("outputBufferLength = %d\r\n", outputBufferLength );
            
            DATA_IRP dataUser;
            dataUser.code =18;
            
            RtlCopyMemory(SystemBuffer, &dataUser, outputBufferLength );
            
            DbgPrint("SystemBuffer->code = %d\r\n", ( (DATA_IRP *) SystemBuffer)->code );
            
            Irp->IoStatus.Status = STATUS_SUCCESS;
            Irp->IoStatus.Information = inputBufferLength;
            break;
        
        default:
            break;
    }
    
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    
    return Irp->IoStatus.Status;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
    
    NTSTATUS status;

    
    DbgPrint("Start DriverMinGW\n");


    DriverObject->DriverUnload = UnloadRoutine;
    //DriverObject->DriverExtension->AddDevice = AddDevice;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControle;

    
    registryPath.Buffer = (PWSTR) ExAllocatePool(PagedPool, RegistryPath->Length + sizeof(WCHAR));

    if(!registryPath.Buffer) {
        DbgPrint("Error registryPath.Buffer: STATUS_INSUFFICIENT_RESOURCES\n");
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    registryPath.MaximumLength = RegistryPath->MaximumLength;
    RtlCopyUnicodeString(&registryPath,RegistryPath);

    RtlInitUnicodeString(&deviceName, L"\\Device\\DriverMinGW");
    RtlInitUnicodeString(&dosDevicesName, L"\\DosDevices\\DriverMinGW");
    DbgPrint("deviceName: %wZ\n",deviceName);
    DbgPrint("dosDevicesName: %wZ\n",dosDevicesName);
    
    status = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &fdo );
    
    if (!NT_SUCCESS(status)) 
    {
        DbgPrint("Error IoCreateDevice: %d\n", status);
        return status;
    }
    
    
    fdo->Flags |= DO_DIRECT_IO;
    fdo->Flags &= ~DO_DEVICE_INITIALIZING;
 
    status = IoCreateSymbolicLink(&dosDevicesName, &deviceName);
    
       
    if (!NT_SUCCESS(status)) 
    {
        DbgPrint("Error IoCreateSymbolicLink: %d\n", status);
        IoDeleteDevice(fdo);
        return status;
    }
    
    return STATUS_SUCCESS;
}
 
VOID UnloadRoutine(IN PDRIVER_OBJECT DriverObject)
{
  RtlFreeUnicodeString(&registryPath);
  IoDeleteSymbolicLink(&dosDevicesName);
  IoDeleteDevice(fdo);
  DbgPrint("End DriverMinGW\n");
}
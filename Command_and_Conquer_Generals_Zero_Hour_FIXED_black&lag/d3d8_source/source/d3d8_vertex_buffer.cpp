#include "d3d8x.hpp"

Direct3DVertexBuffer8::Direct3DVertexBuffer8(Direct3DDevice8 *Device, IDirect3DVertexBuffer9 *ProxyInterface) :
	Device(Device), ProxyInterface(ProxyInterface)
{
	Device->ProxyAddressLookupTable->SaveAddress(this, ProxyInterface);
}
Direct3DVertexBuffer8::~Direct3DVertexBuffer8()
{
}

HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::QueryInterface(REFIID riid, void **ppvObj)
{
	if (ppvObj == nullptr)
		return E_POINTER;

	if (riid == __uuidof(IDirect3DVertexBuffer8) ||
		riid == __uuidof(IUnknown) ||
		riid == __uuidof(IDirect3DResource8))
	{
		AddRef();
		*ppvObj = static_cast<IDirect3DVertexBuffer8 *>(this);

		return S_OK;
	}

	const HRESULT hr = ProxyInterface->QueryInterface(ConvertREFIID(riid), ppvObj);
	if (SUCCEEDED(hr))
		GenericQueryInterface(riid, ppvObj, Device);

	return hr;
}
ULONG STDMETHODCALLTYPE Direct3DVertexBuffer8::AddRef()
{
	return ProxyInterface->AddRef();
}
ULONG STDMETHODCALLTYPE Direct3DVertexBuffer8::Release()
{
	return ProxyInterface->Release();
}

HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::GetDevice(IDirect3DDevice8 **ppDevice)
{
	if (ppDevice == nullptr)
		return D3DERR_INVALIDCALL;

	Device->AddRef();
	*ppDevice = Device;

	return D3D_OK;
}
HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::SetPrivateData(REFGUID refguid, const void *pData, DWORD SizeOfData, DWORD Flags)
{
	return ProxyInterface->SetPrivateData(refguid, pData, SizeOfData, Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::GetPrivateData(REFGUID refguid, void *pData, DWORD *pSizeOfData)
{
	return ProxyInterface->GetPrivateData(refguid, pData, pSizeOfData);
}
HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::FreePrivateData(REFGUID refguid)
{
	return ProxyInterface->FreePrivateData(refguid);
}
DWORD STDMETHODCALLTYPE Direct3DVertexBuffer8::SetPriority(DWORD PriorityNew)
{
	return ProxyInterface->SetPriority(PriorityNew);
}
DWORD STDMETHODCALLTYPE Direct3DVertexBuffer8::GetPriority()
{
	return ProxyInterface->GetPriority();
}
void STDMETHODCALLTYPE Direct3DVertexBuffer8::PreLoad()
{
	ProxyInterface->PreLoad();
}
D3DRESOURCETYPE STDMETHODCALLTYPE Direct3DVertexBuffer8::GetType()
{
	return D3DRTYPE_VERTEXBUFFER;
}

HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::Lock(UINT OffsetToLock, UINT SizeToLock, BYTE **ppbData, DWORD Flags)
{
	if ((Flags & D3DLOCK_DISCARD) != 0)
	{
		D3DVERTEXBUFFER_DESC desc;
		ProxyInterface->GetDesc(&desc);

		if ((desc.Usage & D3DUSAGE_DYNAMIC) == 0 ||
			(desc.Usage & D3DUSAGE_WRITEONLY) == 0)
		{
			Flags ^= D3DLOCK_DISCARD;
		}
	}

	return ProxyInterface->Lock(OffsetToLock, SizeToLock, reinterpret_cast<void **>(ppbData), Flags);
}
HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::Unlock()
{
	return ProxyInterface->Unlock();
}
HRESULT STDMETHODCALLTYPE Direct3DVertexBuffer8::GetDesc(D3DVERTEXBUFFER_DESC *pDesc)
{
	return ProxyInterface->GetDesc(pDesc);
}

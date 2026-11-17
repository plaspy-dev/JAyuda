// TemplateView.h: interfaz de la clase CTemplateView
//


#pragma once


class CTemplateView : public CFormView
{
protected: // Crear sólo a partir de serialización
	CTemplateView();
	DECLARE_DYNCREATE(CTemplateView)

public:
	enum{ IDD = IDD_TEMPLATE_FORM };

// Operaciones
public:

// Reemplazos
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Compatibilidad con DDX/DDV
	virtual void OnInitialUpdate(); // Se llama la primera vez después de la construcción

// Implementación
public:
	virtual ~CTemplateView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Funciones de asignación de mensajes generadas
protected:
	DECLARE_MESSAGE_MAP()
};



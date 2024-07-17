#include "seampaintingoperatemode.h"
#include "interface/selectorinterface.h"
#include "data/modeln.h"
#include "interface/eventinterface.h"
#include "interface/visualsceneinterface.h"
#include "interface/spaceinterface.h"
#include "kernel/kernel.h"
#include "kernel/globalconst.h"
#include "kernel/kernelui.h"

SeamPaintingOperateMode::SeamPaintingOperateMode(QObject* parent)
    : PaintOperateMode(parent)
{
	m_colorMethod = 1;
}

SeamPaintingOperateMode::~SeamPaintingOperateMode()
{

}

void SeamPaintingOperateMode::initColors()
{
    creative_kernel::EngineType type = creative_kernel::getKernel()->globalConst()->getEngineType();
     if (type == creative_kernel::EngineType::ET_CURA) 
     {   /* ET_CURA */
        std::vector<trimesh::vec> colors;
        colors.push_back(trimesh::vec(0.87, 0.87, 0.87));
        colors.push_back(trimesh::vec(0.32, 0.76, 0.32));
        colors.push_back(trimesh::vec(0.76, 0.32, 0.32));
        setColorsList(colors);
        
        m_paintIndex = 1;
        m_blockIndex = 2;
        setDefaultFlag(1);
     }
     else if (type == creative_kernel::EngineType::ET_ORCA)
     {   /* ET_ORCA */
         std::vector<trimesh::vec> colors;
         colors.push_back(trimesh::vec(0.32, 0.76, 0.32));
         colors.push_back(trimesh::vec(0.76, 0.32, 0.32));
         colors.push_back(trimesh::vec(0.87, 0.87, 0.87));
         setColorsList(colors);

         m_paintIndex = 0;
         m_blockIndex = 1;
         setDefaultFlag(3);
     }
}

void SeamPaintingOperateMode::restore(creative_kernel::ModelN* model, const std::vector<std::string>& data)
{
    if (!m_originModel)
	{	// 切换到涂抹场景
        creative_kernel::selectOne(model);
        setWaitForLoadModel(true);
        getKernelUI()->switchMode(31);
        creative_kernel::setVisOperationMode(this);
        setWaitForLoadModel(false);
	}

    PaintOperateMode::restore(model, data);
}

void SeamPaintingOperateMode::onAttach() 
{	
    auto selections = creative_kernel::selectionms();
	if (selections.isEmpty())
		return;
    m_lastSpreadData = selections.at(0)->getSeam2Facets();
    
    PaintOperateMode::onAttach();
    m_isWrapperReady = true;
    initColors();
}

void SeamPaintingOperateMode::onDettach() 
{
    if (m_originModel)
    {
        auto oldData = m_originModel->getSeam2Facets();
        auto newData = getPaintData();

        if (!paintDataEqual(oldData, newData))
        {
            m_originModel->setSeam2Facets(newData);
            m_originModel->dirty();
        }

        PaintOperateMode::onDettach();
    }
}

void SeamPaintingOperateMode::onLeftMouseButtonPress(QMouseEvent* event)
{
    setColorIndex(m_paintIndex);
    PaintOperateMode::onLeftMouseButtonPress(event);
}

void SeamPaintingOperateMode::onRightMouseButtonPress(QMouseEvent* event)
{
    if (isHoverModel(event->pos()))
    {
        m_pressed = true;
        setColorIndex(m_blockIndex);
        PaintOperateMode::onLeftMouseButtonPress(event);
        m_rightPressStatus = false;
    }
    else 
    {
        PaintOperateMode::onRightMouseButtonPress(event);
    }
}

void SeamPaintingOperateMode::onRightMouseButtonMove(QMouseEvent* event)
{
    if (m_pressed)
    {
        PaintOperateMode::onLeftMouseButtonMove(event);
        m_rightMoveStatus = false;
    }
    else 
    {
        PaintOperateMode::onRightMouseButtonMove(event);
    }
}

void SeamPaintingOperateMode::onRightMouseButtonRelease(QMouseEvent* event)
{
    m_pressed = false;
    PaintOperateMode::onLeftMouseButtonRelease(event);
}
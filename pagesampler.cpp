#include "pagesampler.h"
#include "mainwindow.h"
#include "ui_pagesampler.h"
#include "tiny_gltf.h"
#include "gltfinfo.h"

namespace
{
QString filterName(int filter)
{
    switch(filter)
    {
    case TINYGLTF_TEXTURE_FILTER_NEAREST: return "NEAREST";
    case TINYGLTF_TEXTURE_FILTER_LINEAR: return "LINEAR";
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST: return "NEAREST_MIPMAP_NEAREST";
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST: return "LINEAR_MIPMAP_NEAREST";
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR: return "NEAREST_MIPMAP_LINEAR";
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR: return "LINEAR_MIPMAP_LINEAR";
    default: return "unknown";
    }
}

QString filterText(int filter)
{
    if(filter < 0) return "not set";
    return QString("%1 (%2)").arg(filterName(filter)).arg(filter);
}

QString wrapName(int wrap)
{
    switch(wrap)
    {
    case TINYGLTF_TEXTURE_WRAP_REPEAT: return "REPEAT";
    case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE: return "CLAMP_TO_EDGE";
    case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT: return "MIRRORED_REPEAT";
    default: return "unknown";
    }
}

QString wrapText(int wrap)
{
    return QString("%1 (%2)").arg(wrapName(wrap)).arg(wrap);
}
}

PageSampler::PageSampler(MainWindow *mainWindow)
    : Page(mainWindow)
    , ui(new Ui::PageSampler)
{
    ui->setupUi(this);
}

PageSampler::~PageSampler()
{
    delete ui;
}

void PageSampler::setData(const tinygltf::Model &model, int index)
{
    if(index < 0 || index >= (int)model.samplers.size()) return;

    const tinygltf::Sampler &sampler = model.samplers[index];

    ui->index->setText(QString::number(index));
    ui->name->setText(QString::fromStdString(sampler.name));
    ui->minFilter->setText(filterText(sampler.minFilter));
    ui->magFilter->setText(filterText(sampler.magFilter));
    ui->wrapS->setText(wrapText(sampler.wrapS));
    ui->wrapT->setText(wrapText(sampler.wrapT));
    ui->wrapR->setText(wrapText(sampler.wrapR));

    QStringList extensions;
    for(const auto &extension : sampler.extensions)
        extensions << QString::fromStdString(extension.first);
    extensions.sort();
    ui->extensions->setText(extensions.isEmpty() ? "None" : extensions.join(", "));
}

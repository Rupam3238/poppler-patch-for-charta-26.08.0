#include <cmath>
#include <map>
#include <memory>
#include <sstream>

#include <QtTest/QTest>
#include <QTemporaryFile>

#include <poppler-qt6.h>

#include "poppler/Annot.h"
#include "poppler/PDFDoc.h"
#include "poppler/PDFDocFactory.h"
#include "goo/GooString.h"
#include "goo/gstrtod.h"

class TestAnnotations : public QObject
{
    Q_OBJECT
public:
    explicit TestAnnotations(QObject *parent = nullptr) : QObject(parent) { }

    static void saveAndCheck(const std::unique_ptr<Poppler::Document> &doc, const std::function<void(Poppler::Annotation *a)> &checkFunction);

private Q_SLOTS:
    static void checkQColorPrecision();
    static void checkFontSizeAndColor();
    static void checkHighlightFromAndToQuads();
    static void checkUTF16LEAnnot();
    static void checkModificationCreationDate();
    static void checkNonMarkupAnnotations();
    static void checkDefaultAppearance();
    static void checkAnnotationReorderRenderingAndSave();
};

/* Is .5f sufficient for 16 bit color channel roundtrip trough save and load on all architectures? */
void TestAnnotations::checkQColorPrecision()
{
    bool precisionOk = true;
    for (uint32_t i = std::numeric_limits<uint16_t>::min(); i <= std::numeric_limits<uint16_t>::max(); i++) {
        double normalized = i / static_cast<double>(std::numeric_limits<uint16_t>::max());
        const std::string serialized = GooString::format("{0:.5f}", normalized);
        double deserialized = gatof(serialized.c_str());
        uint16_t denormalized = std::round(deserialized * std::numeric_limits<uint16_t>::max());
        if (i != denormalized) {
            precisionOk = false;
            break;
        }
    }
    QVERIFY(precisionOk);
}

void TestAnnotations::checkFontSizeAndColor()
{
    const QString contents = QStringLiteral("foobar");
    const std::vector<QColor> testColors { QColor::fromRgb(0xAB, 0xCD, 0xEF), QColor::fromCmyk(0xAB, 0xBC, 0xCD, 0xDE) };
    const QFont testFont(QStringLiteral("Helvetica"), 20);

    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    tempFile.close();

    {
        std::unique_ptr<Poppler::Document> doc { Poppler::Document::load(QStringLiteral(TESTDATADIR "/unittestcases/UseNone.pdf")) };
        QVERIFY(doc.get());

        std::unique_ptr<Poppler::Page> page { doc->page(0) };
        QVERIFY(page.get());

        for (const auto &color : testColors) {
            auto annot = std::make_unique<Poppler::TextAnnotation>(Poppler::TextAnnotation::InPlace);
            annot->setBoundary(QRectF(0.0, 0.0, 1.0, 1.0));
            annot->setContents(contents);
            annot->setTextFont(testFont);
            annot->setTextColor(color);
            page->addAnnotation(annot.get());
        }

        std::unique_ptr<Poppler::PDFConverter> conv(doc->pdfConverter());
        QVERIFY(conv.get());
        conv->setOutputFileName(tempFile.fileName());
        conv->setPDFOptions(Poppler::PDFConverter::WithChanges);
        QVERIFY(conv->convert());
    }

    {
        std::unique_ptr<Poppler::Document> doc { Poppler::Document::load(tempFile.fileName()) };
        QVERIFY(doc.get());

        std::unique_ptr<Poppler::Page> page { doc->page(0) };
        QVERIFY(page.get());

        auto annots = page->annotations();
        QCOMPARE(annots.size(), static_cast<int>(testColors.size()));

        auto &&annot = annots.cbegin();
        for (const auto &color : testColors) {
            QCOMPARE((*annot)->subType(), Poppler::Annotation::AText);
            auto *textAnnot = static_cast<Poppler::TextAnnotation *>(annot->get());
            QCOMPARE(textAnnot->contents(), contents);
            QCOMPARE(textAnnot->textFont().pointSize(), testFont.pointSize());
            QCOMPARE(static_cast<int>(textAnnot->textColor().spec()), static_cast<int>(color.spec()));
            QCOMPARE(textAnnot->textColor(), color);
            if (annot != annots.cend()) {
                ++annot;
            }
        }
    }
}

namespace Poppler {
static bool operator==(const Poppler::HighlightAnnotation::Quad &a, const Poppler::HighlightAnnotation::Quad &b)
{
    // FIXME We do not compare capStart, capEnd and feather since AnnotQuadrilaterals doesn't contain that info and thus
    //       HighlightAnnotationPrivate::fromQuadrilaterals uses default values
    return a.points[0] == b.points[0] && a.points[1] == b.points[1] && a.points[2] == b.points[2] && a.points[3] == b.points[3];
}
}

void TestAnnotations::checkHighlightFromAndToQuads()
{
    std::unique_ptr<Poppler::Document> doc { Poppler::Document::load(QStringLiteral(TESTDATADIR "/unittestcases/UseNone.pdf")) };

    std::unique_ptr<Poppler::Page> page { doc->page(0) };

    auto ha = std::make_unique<Poppler::HighlightAnnotation>();
    page->addAnnotation(ha.get());

    const QList<Poppler::HighlightAnnotation::Quad> quads = { { .points = { { 0, 0.1 }, { 0.2, 0.3 }, { 0.4, 0.5 }, { 0.6, 0.7 } }, .capStart = false, .capEnd = false, .feather = 0 },
                                                              { .points = { { 0.8, 0.9 }, { 0.1, 0.2 }, { 0.3, 0.4 }, { 0.5, 0.6 } }, .capStart = true, .capEnd = false, .feather = 0.4 } };
    ha->setHighlightQuads(quads);
    QCOMPARE(ha->highlightQuads(), quads);
}

void TestAnnotations::checkUTF16LEAnnot()
{
    std::unique_ptr<Poppler::Document> doc { Poppler::Document::load(QStringLiteral(TESTDATADIR "/unittestcases/utf16le-annot.pdf")) };
    QVERIFY(doc.get());

    std::unique_ptr<Poppler::Page> page { doc->page(0) };
    QVERIFY(page.get());

    auto annots = page->annotations();
    QCOMPARE(annots.size(), 2);

    const auto &annot = annots[1];
    QCOMPARE(annot->contents(), QString::fromUtf8("Únîcödé豰")); // clazy:exclude=qstring-allocations
}

void TestAnnotations::saveAndCheck(const std::unique_ptr<Poppler::Document> &doc, const std::function<void(Poppler::Annotation *a)> &checkFunction)
{
    // also check that saving yields the same output
    QTemporaryFile tempFile;
    QVERIFY(tempFile.open());
    tempFile.close();

    std::unique_ptr<Poppler::PDFConverter> conv(doc->pdfConverter());
    conv->setOutputFileName(tempFile.fileName());
    conv->setPDFOptions(Poppler::PDFConverter::WithChanges);
    conv->convert();

    std::unique_ptr<Poppler::Document> savedDoc { Poppler::Document::load(tempFile.fileName()) };
    std::unique_ptr<Poppler::Page> page { doc->page(0) };
    auto annots = page->annotations();
    checkFunction(annots.at(1).get());
}

void TestAnnotations::checkModificationCreationDate()
{
    std::unique_ptr<Poppler::Document> doc { Poppler::Document::load(QStringLiteral(TESTDATADIR "/unittestcases/utf16le-annot.pdf")) };
    QVERIFY(doc.get());

    std::unique_ptr<Poppler::Page> page { doc->page(0) };

    auto annots = page->annotations();
    const auto &annot = annots.at(1);
    QCOMPARE(annot->creationDate(), QDateTime());
    QCOMPARE(annot->modificationDate(), QDateTime());

    const QDateTime dt1(QDate(2020, 8, 7), QTime(18, 34, 56));
    annot->setCreationDate(dt1);
    auto checkFunction1 = [dt1](Poppler::Annotation *a) {
        QCOMPARE(a->creationDate(), dt1);
        // setting the creation date updates the modification date
        QVERIFY(std::abs(a->modificationDate().secsTo(QDateTime::currentDateTime())) < 2);
    };
    checkFunction1(annot.get());
    saveAndCheck(doc, checkFunction1);

    const QDateTime dt2(QDate(2020, 8, 30), QTime(8, 14, 52));
    annot->setModificationDate(dt2);
    auto checkFunction2 = [dt2](Poppler::Annotation *a) { QCOMPARE(a->modificationDate(), dt2); };
    checkFunction2(annot.get());
    saveAndCheck(doc, checkFunction2);

    // setting the creation date to empty means "use the modification date" and also updates the modification date
    // so both creation date and modification date are the same and are now
    annot->setCreationDate(QDateTime());
    auto checkFunction3 = [](Poppler::Annotation *a) {
        QVERIFY(std::abs(a->creationDate().secsTo(QDateTime::currentDateTime())) < 2);
        QCOMPARE(a->creationDate(), a->modificationDate());
    };
    checkFunction3(annot.get());
    saveAndCheck(doc, checkFunction3);

    annot->setModificationDate(QDateTime());
    auto checkFunction4 = [](Poppler::Annotation *a) {
        QCOMPARE(a->creationDate(), QDateTime());
        QCOMPARE(a->modificationDate(), QDateTime());
    };
    checkFunction4(annot.get());
    saveAndCheck(doc, checkFunction4);
}

void TestAnnotations::checkNonMarkupAnnotations()
{
    std::unique_ptr<Poppler::Document> doc { Poppler::Document::load(QStringLiteral(TESTDATADIR "/unittestcases/checkbox_issue_159.pdf")) };
    QVERIFY(doc.get());

    std::unique_ptr<Poppler::Page> page { doc->page(0) };
    QVERIFY(page.get());

    auto annots = page->annotations();
    QCOMPARE(annots.size(), 17);
}

void TestAnnotations::checkDefaultAppearance()
{
    std::string roundtripString;
    {
        const std::string daString { "/Helv 10 Tf 0.1 0.2 0.3 rg" };
        const DefaultAppearance da { daString };
        QCOMPARE(da.getFontPtSize(), 10.);
        QVERIFY(!da.getFontName().empty());
        QCOMPARE(da.getFontName(), "Helv");
        const AnnotColor *color = da.getFontColor();
        QVERIFY(color);
        QCOMPARE(color->getSpace(), AnnotColor::colorRGB);
        QCOMPARE(color->getValues()[0], 0.1);
        QCOMPARE(color->getValues()[1], 0.2);
        QCOMPARE(color->getValues()[2], 0.3);
        roundtripString = da.toAppearanceString();
    }
    {
        /* roundtrip through parse/generate/parse shall preserve values */
        const DefaultAppearance da { roundtripString };
        QCOMPARE(da.getFontPtSize(), 10.);
        QVERIFY(!da.getFontName().empty());
        QCOMPARE(da.getFontName(), "Helv");
        const AnnotColor *color = da.getFontColor();
        QVERIFY(color);
        QCOMPARE(color->getSpace(), AnnotColor::colorRGB);
        QCOMPARE(color->getValues()[0], 0.1);
        QCOMPARE(color->getValues()[1], 0.2);
        QCOMPARE(color->getValues()[2], 0.3);
    }
    {
        /* parsing bad DA strings must not cause crash */
        const std::string daString { "/ % Tf 1 2 rg" };
        const DefaultAppearance da { daString };
        QVERIFY(da.getFontName().empty());
    }
}

void TestAnnotations::checkAnnotationReorderRenderingAndSave()
{
    std::unique_ptr<Poppler::Document> doc { Poppler::Document::load(QStringLiteral(TESTDATADIR "/simple-multipage.pdf")) };
    QVERIFY(doc.get());

    std::unique_ptr<Poppler::Page> page { doc->page(0) };
    QVERIFY(page.get());

    auto red = std::make_unique<Poppler::GeomAnnotation>();
    red->setUniqueName(QStringLiteral("stack-red"));
    red->setBoundary(QRectF(0.2, 0.2, 0.6, 0.6));
    red->setGeomType(Poppler::GeomAnnotation::InscribedSquare);
    red->setGeomInnerColor(Qt::red);

    auto blue = std::make_unique<Poppler::GeomAnnotation>();
    blue->setUniqueName(QStringLiteral("stack-blue"));
    blue->setBoundary(QRectF(0.2, 0.2, 0.6, 0.6));
    blue->setGeomType(Poppler::GeomAnnotation::InscribedSquare);
    blue->setGeomInnerColor(Qt::blue);

    auto ink = std::make_unique<Poppler::InkAnnotation>();
    ink->setUniqueName(QStringLiteral("stack-custom-ink"));
    ink->setBoundary(QRectF(0.05, 0.05, 0.1, 0.1));
    ink->setInkPaths({{QPointF(0.05, 0.05), QPointF(0.15, 0.15)}});
    ink->setAppearanceStream(QByteArray("0 1 0 rg 0 0 100 100 re f"), QRectF(0, 0, 100, 100));

    page->addAnnotation(red.get());
    page->addAnnotation(blue.get());
    page->addAnnotation(ink.get());
    auto annotations = page->annotations();
    QCOMPARE(annotations.size(), size_t(3));
    QCOMPARE(annotations.at(1)->uniqueName(), QStringLiteral("stack-blue"));

    QImage image = page->renderToImage();
    QVERIFY(!image.isNull());
    const QPoint samplePoint(image.width() / 2, image.height() / 2);
    QCOMPARE(image.pixelColor(samplePoint), QColor(Qt::blue));

    QTemporaryFile beforeMoveFile;
    QVERIFY(beforeMoveFile.open());
    beforeMoveFile.close();
    std::unique_ptr<Poppler::PDFConverter> beforeMoveConverter(doc->pdfConverter());
    beforeMoveConverter->setOutputFileName(beforeMoveFile.fileName());
    beforeMoveConverter->setPDFOptions(beforeMoveConverter->pdfOptions() | Poppler::PDFConverter::WithChanges);
    QVERIFY(beforeMoveConverter->convert());

    using AnnotationRefs = std::map<std::string, std::pair<int, int>>;
    const auto annotationRefs = [](const QString &fileName) {
        const GooString nativeFileName(fileName.toStdString());
        std::unique_ptr<PDFDoc> nativeDoc = PDFDocFactory().createPDFDoc(nativeFileName);
        AnnotationRefs refs;
        if (!nativeDoc) {
            return refs;
        }
        const auto &nativeAnnotations = nativeDoc->getPage(1)->getAnnots()->getAnnots();
        for (const std::shared_ptr<Annot> &nativeAnnotation : nativeAnnotations) {
            const GooString *name = nativeAnnotation->getUniqueName();
            if (name) {
                const Ref ref = nativeAnnotation->getRef();
                refs.emplace(name->toStr(), std::pair { ref.num, ref.gen });
            }
        }
        return refs;
    };
    const AnnotationRefs refsBeforeMove = annotationRefs(beforeMoveFile.fileName());
    QCOMPARE(refsBeforeMove.size(), size_t(3));

    QVERIFY(page->moveAnnotation(red.get(), 1));
    annotations = page->annotations();
    QCOMPARE(annotations.size(), size_t(3));
    QCOMPARE(annotations.at(1)->uniqueName(), QStringLiteral("stack-red"));

    image = page->renderToImage();
    QCOMPARE(image.pixelColor(samplePoint), QColor(Qt::red));

    QTemporaryFile outputFile;
    QVERIFY(outputFile.open());
    outputFile.close();
    std::unique_ptr<Poppler::PDFConverter> converter(doc->pdfConverter());
    QVERIFY(converter.get());
    converter->setOutputFileName(outputFile.fileName());
    converter->setPDFOptions(converter->pdfOptions() | Poppler::PDFConverter::WithChanges);
    QVERIFY(converter->convert());

    const AnnotationRefs refsAfterMove = annotationRefs(outputFile.fileName());
    QCOMPARE(refsAfterMove, refsBeforeMove);

    std::unique_ptr<Poppler::Document> reopened { Poppler::Document::load(outputFile.fileName()) };
    QVERIFY(reopened.get());
    std::unique_ptr<Poppler::Page> reopenedPage { reopened->page(0) };
    QVERIFY(reopenedPage.get());
    const auto reopenedAnnotations = reopenedPage->annotations();
    QCOMPARE(reopenedAnnotations.size(), size_t(3));
    QCOMPARE(reopenedAnnotations.at(1)->uniqueName(), QStringLiteral("stack-red"));
    QCOMPARE(reopenedAnnotations.back()->uniqueName(), QStringLiteral("stack-custom-ink"));
    const QImage reopenedImage = reopenedPage->renderToImage();
    QCOMPARE(reopenedImage.pixelColor(samplePoint), QColor(Qt::red));
    const QRect customInkRegion(qRound(reopenedImage.width() * 0.04), qRound(reopenedImage.height() * 0.04), qRound(reopenedImage.width() * 0.12), qRound(reopenedImage.height() * 0.12));
    bool foundCustomInkGreen = false;
    for (int y = customInkRegion.top(); y <= customInkRegion.bottom() && !foundCustomInkGreen; ++y) {
        for (int x = customInkRegion.left(); x <= customInkRegion.right(); ++x) {
            const QColor color = reopenedImage.pixelColor(x, y);
            if (color.green() > 200 && color.red() < 40 && color.blue() < 40) {
                foundCustomInkGreen = true;
                break;
            }
        }
    }
    QVERIFY(foundCustomInkGreen);
}

QTEST_GUILESS_MAIN(TestAnnotations)

#include "check_annotations.moc"

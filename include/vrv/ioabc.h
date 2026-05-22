/////////////////////////////////////////////////////////////////////////////
// Name:        ioabc.h
// Author:      Klaus Rettinghaus
// Created:     2017
// Copyright (c) Klaus Rettinghaus. All rights reserved.
/////////////////////////////////////////////////////////////////////////////

#ifndef __VRV_IOABC_H__
#define __VRV_IOABC_H__

#include <sstream>
#include <string>
#include <vector>

//----------------------------------------------------------------------------

#include "iobase.h"
#include "pugixml.hpp"
#include "vrvdef.h"

namespace vrv {

class BarLine;
class Beam;
class Clef;
class ControlElement;
class Harm;
class KeySig;
class Layer;
class LayerElement;
class Mdiv;
class Measure;
class MeterSig;
class Note;
class Score;
class Section;
class Slur;
class Staff;
class Tie;

//----------------------------------------------------------------------------
// ABCInput
//----------------------------------------------------------------------------

class ABCInput : public Input {
public:
    // constructors and destructors
    ABCInput(Doc *doc);
    virtual ~ABCInput();

    bool Import(const std::string &abc) override;

#ifndef NO_ABC_SUPPORT

private:
    // function declarations:

    void ParseABC(std::istream &infile);

    // parsing functions
    int SetBarLine(const std::string &musicCode, int index);
    void CalcUnitNoteLength();
    void AddAnnot(const std::string &remark);
    void AddLayerElement();
    void AddTie();
    void StartSlur();
    void EndSlur();
    int ParseTuplet(const std::string &musicCode, int index);

    // parse information fields
    void ParseInstruction(const std::string &keyString); // I:
    void ParseKey(std::string &keyString); // K:
    void ParseUnitNoteLength(const std::string &unitNoteLength); // L:
    void ParseMeter(const std::string &meterString); // M:
    void ParseTempo(const std::string &tempoString); // Q:
    void ParseReferenceNumber(const std::string &referenceNumberString); // X:
    void ParseLyrics(); // w:

    // input functions
    void ReadInformationField(const char &dataKey, std::string dataValue);
    void ReadMusicCode(const std::string &musicCode, Section *section);

    // decoration functions
    void ParseDecoration(const std::string &decorationString);
    void AddArticulation(LayerElement *element);
    void AddChordSymbol(LayerElement *element);
    void AddDynamic(LayerElement *element);
    void AddFermata(LayerElement *element);
    void AddOrnaments(LayerElement *element);
    void AddRepeatMark(LayerElement *element);

    // additional functions
    void PrintInformationFields(Score *score);
    void CreateHeader();
    void CreateWorkEntry();
    void FlushControlElements(Score *score, Section *section);
    void InitScoreAndSection(Score *&score, Section *&section);

#endif // NO_ABC_SUPPORT

public:
    //
private:
    enum class ElementType { Default, Tuplet };
    struct ContainerElement {
        ElementType m_type = ElementType::Default;
        LayerElement *m_element = NULL;
        int m_count = 0;
    };

    std::string m_filename;
    Mdiv *m_mdiv = NULL;
    Clef *m_clef = NULL;
    KeySig *m_key = NULL;
    MeterSig *m_meter = NULL;
    Layer *m_layer = NULL;

    data_DURATION m_durDefault;
    std::string m_ID;
    int m_unitDur;
    std::pair<data_BARRENDITION, data_BARRENDITION> m_barLines
        = std::make_pair(BARRENDITION_NONE, BARRENDITION_NONE); //
    /*
     * ABC variables with default values
     */
    char m_decoration = '!';
    char m_linebreak = '$';
    int m_lineNum = 1;
    int m_broken = 0;
    int m_gracecount = 0;
    int m_stafflines = 5;
    int m_transpose = 0;
    ContainerElement m_containerElement;
    /*
     * ABC metadata stacks
     */
    std::vector<std::pair<std::string, int>> m_composer; // C:
    std::vector<std::pair<std::string, int>> m_history; // H:
    std::vector<std::pair<std::string, int>> m_notes; // N:
    std::vector<std::pair<std::string, int>> m_origin; // O:
    std::vector<std::pair<std::string, int>> m_title; // T:
    std::vector<std::pair<std::pair<std::string, int>, char>> m_info;

    std::vector<ControlElement *> m_tempoStack;
    std::vector<Harm *> m_harmStack;
    std::vector<Slur *> m_slurStack;
    std::vector<Tie *> m_tieStack;

    std::vector<LayerElement *> m_layerElements;
    std::vector<LayerElement *> m_noteStack;
    // Array of added notes in one line of ABC file. Used to track elements that might require adding verse to
    std::vector<LayerElement *> m_lineNoteArray;
    int m_verseNumber = 1;
    /*
     * ABC decoration stacks
     */
    std::vector<data_ARTICULATION> m_artic;
    std::vector<std::string> m_dynam;
    std::string m_ornam;
    data_STAFFREL m_fermata = STAFFREL_NONE;
    repeatMarkLog_FUNC m_repeatMark = repeatMarkLog_FUNC_NONE;
    /*
     * The stack of control elements to be added at the end of each measure
     */
    std::vector<std::pair<std::string, ControlElement *>> m_controlElements;
    /*
     * container for work entries
     */
    pugi::xml_node m_workList;
};

//----------------------------------------------------------------------------
// Forward declarations for ABCOutput
//----------------------------------------------------------------------------
class Accid;
class Chord;
class GraceGrp;
class MRest;
class MultiRest;
class Rest;
class ScoreDef;
class StaffDef;
class Tuplet;

//----------------------------------------------------------------------------
// ABCOutput
//----------------------------------------------------------------------------

/**
 * This class is a file output stream for writing ABC notation files.
 * Unit note length is L:1/8 (eighth note).
 */
class ABCOutput : public Output {
public:
    ABCOutput(Doc *doc);
    virtual ~ABCOutput();

    /**
     * The main Export method - returns the complete ABC string.
     */
    std::string Export() override;

    /**
     * @name WriteObject / WriteObjectEnd called during tree walk.
     */
    ///@{
    bool WriteObject(Object *object) override;
    bool WriteObjectEnd(Object *object) override;
    ///@}

private:
    // ── Scordef / header collection ──────────────────────────────────────────
    void WriteStaffDef(StaffDef *staffDef);
    void WriteKeySig(KeySig *keySig);
    void WriteMeterSig(MeterSig *meterSig);

    // ── Body ─────────────────────────────────────────────────────────────────
    void WriteMeasure(Measure *measure);
    void WriteMeasureEnd(Measure *measure);
    void WriteStaff(Staff *staff);
    void WriteLayer(Layer *layer);
    void WriteNote(Note *note);
    void WriteRest(Rest *rest);
    void WriteMRest(MRest *mRest);
    void WriteMultiRest(MultiRest *multiRest);
    void WriteChord(Chord *chord);
    void WriteChordEnd(Chord *chord);
    void WriteTuplet(Tuplet *tuplet);

    // ── Helpers ───────────────────────────────────────────────────────────────
    std::string PitchToABC(data_PITCHNAME pname, int oct) const;
    std::string AccidToABC(data_ACCIDENTAL_WRITTEN accid) const;
    std::string DurationToABC(data_DURATION dur, int dots) const;
    std::string KeySigToABCKey(int count, data_ACCIDENTAL_WRITTEN type) const;
    static int GCD(int a, int b);

    std::ostringstream m_out;
    bool m_docScoreDef; ///< True while processing the ScoreDef first-pass
    bool m_skip;        ///< Skip non-primary staves / layers
    int m_staffN;       ///< N of the first (primary) staff
    int m_layerN;       ///< N of the first (primary) layer
    bool m_inChord;     ///< Currently inside a Chord element
    data_DURATION m_chordDur;
    int m_chordDots;
    Measure *m_currentMeasure;
    bool m_firstMeasure;

    // Collected header information
    std::string m_title;
    std::string m_key;
    std::string m_meter;
    int m_tempoNum;
    bool m_hasTempo;
};

} // namespace vrv

#endif

#pragma once
#include <xlsxwriter.h>

#include <QDebug>
#include <string>

class ExcelWriter
{
public:
    ExcelWriter(const std::string &filename);
    ~ExcelWriter();

    void createWorkSheet(const std::string &sheetName);
    void writeCell(int row, int col, const std::string &text) noexcept;
    void writeCell(int row, int col, double number) noexcept;
    void mergeCells(int startRow, int startCol, int endRow, int endCol, const std::string &text) noexcept;
    void setCellStyle(int row, int col, const std::string &fontName, bool bold, bool italic, bool underline,
                      int alignment, int border) noexcept;
    void save() noexcept;

private:
    lxw_workbook *workbook;
    lxw_worksheet *worksheet;
    std::string filename;
};

#include <stdexcept>

#include "ExcelWriter.h"
ExcelWriter::ExcelWriter(const std::string &filename) : workbook(nullptr), worksheet(nullptr), filename(filename)
{
    workbook = workbook_new(filename.c_str());
    if (!workbook)
    {
        throw std::runtime_error("Failed to create workbook");
    }
}

ExcelWriter::~ExcelWriter()
{
    if (workbook)
    {
        workbook_close(workbook);
    }
}

void ExcelWriter::createWorkSheet(const std::string &sheetName)
{
    worksheet = workbook_add_worksheet(workbook, sheetName.c_str());
    if (!worksheet)
    {
        throw std::runtime_error("Failed to create worksheet");
    }
}

void ExcelWriter::writeCell(int row, int col, const std::string &data) noexcept
{
    if (worksheet)
    {
        lxw_format *format = workbook_add_format(workbook);
        worksheet_set_column(worksheet, col, col, 17, format);
        worksheet_write_string(worksheet, row, col, data.c_str(), format);
    }
}

void ExcelWriter::mergeCells(int startRow, int startCol, int endRow, int endCol, const std::string &data) noexcept
{
    if (worksheet)
    {
        lxw_format *format = workbook_add_format(workbook);
        format_set_align(format, LXW_ALIGN_CENTER);
        format_set_font_size(format, 20);
        worksheet_set_row(worksheet, endRow, 30, format);
        worksheet_merge_range(worksheet, startRow, startCol, endRow, endCol, data.c_str(), format);
    }
}

void ExcelWriter::setCellStyle(int row, int col, const std::string &fontName, bool bold, bool italic, bool underline,
                               int alignment, int border) noexcept
{
    if (!worksheet)
        return;

    lxw_format *format = workbook_add_format(workbook);
    if (bold)
        format_set_bold(format);
    if (italic)
        format_set_italic(format);
    if (underline)
        format_set_underline(format, LXW_UNDERLINE_SINGLE);
    format_set_font_name(format, fontName.c_str());

    if (alignment == 1)
    {
        format_set_align(format, LXW_ALIGN_CENTER);
    }
    else if (alignment == 2)
    {
        format_set_align(format, LXW_ALIGN_RIGHT);
    }
    else
    {
        format_set_align(format, LXW_ALIGN_LEFT);
    }

    if (border)
    {
        format_set_border(format, LXW_BORDER_THIN);
    }

    worksheet_write_string(worksheet, row, col, "", format); // Apply style on empty cell
}

void ExcelWriter::save() noexcept
{
    if (workbook)
    {
        workbook_close(workbook);
        workbook = nullptr; // Prevents double-closing
    }
}

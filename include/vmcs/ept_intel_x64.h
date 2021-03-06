//
// Bareflank Hypervisor
//
// Copyright (C) 2015 Assured Information Security, Inc.
// Author: Rian Quinn        <quinnr@ainfosec.com>
// Author: Brendan Kerrigan  <kerriganb@ainfosec.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

#ifndef EPT_INTEL_X64_H
#define EPT_INTEL_X64_H

#include <gsl/gsl>

#include <vector>
#include <memory>
#include <vmcs/ept_entry_intel_x64.h>

class ept_intel_x64 : public ept_entry_intel_x64
{
public:

    using pointer = uintptr_t *;
    using integer_pointer = uintptr_t;
    using size_type = std::size_t;

    /// Constructor
    ///
    /// Creates a extended page table, and stores the parent entry that points
    /// to this entry so that you can modify the properties of this extended
    /// page table as needed.
    ///
    /// @expects none
    /// @ensures none
    ///
    /// @param epte the parent extended page table entry that points to this
    ///     table
    ///
    ept_intel_x64(pointer epte = nullptr);

    /// Destructor
    ///
    /// @expects none
    /// @ensures none
    ///
    ~ept_intel_x64() override = default;

    /// Global Size
    ///
    /// @expects none
    /// @ensures none
    ///
    /// @return returns the number of entries in the entire ept
    ///     tree. Note that this function is expensive.
    ///
    size_type global_size() const noexcept;

    /// Add Page (1 Gigabyte Granularity)
    ///
    /// Adds a page to the extended page table structure. Note that this is the
    /// public function, and should only be used to add pages to the
    /// PML4 extended page table. This function will call a private version that
    /// will parse through the different levels making sure the virtual
    /// address provided is valid.
    ///
    /// @expects none
    /// @ensures none
    ///
    /// @param addr the virtual address to the page to add
    /// @return the resulting epte. Note that this epte is blank, and its
    ///     properties should be set by the caller
    ///
    gsl::not_null<ept_entry_intel_x64 *> add_page_1g(integer_pointer addr)
    { return add_page(addr, intel_x64::ept::pml4::from, intel_x64::ept::pdpt::from); }

    /// Add Page (2 Megabyte Granularity)
    ///
    /// Adds a page to the extended page table structure. Note that this is the
    /// public function, and should only be used to add pages to the
    /// PML4 extended page table. This function will call a private version that
    /// will parse through the different levels making sure the virtual
    /// address provided is valid.
    ///
    /// @expects none
    /// @ensures none
    ///
    /// @param addr the virtual address to the page to add
    /// @return the resulting epte. Note that this epte is blank, and its
    ///     properties should be set by the caller
    ///
    gsl::not_null<ept_entry_intel_x64 *> add_page_2m(integer_pointer addr)
    { return add_page(addr, intel_x64::ept::pml4::from, intel_x64::ept::pd::from); }

    /// Add Page (4 Kilobyte Granularity)
    ///
    /// Adds a page to the extended page table structure. Note that this is the
    /// public function, and should only be used to add pages to the
    /// PML4 extended page table. This function will call a private version that
    /// will parse through the different levels making sure the virtual
    /// address provided is valid.
    ///
    /// @expects none
    /// @ensures none
    ///
    /// @param addr the virtual address to the page to add
    /// @return the resulting epte. Note that this epte is blank, and its
    ///     properties should be set by the caller
    ///
    gsl::not_null<ept_entry_intel_x64 *> add_page_4k(integer_pointer addr)
    { return add_page(addr, intel_x64::ept::pml4::from, intel_x64::ept::pt::from); }

    /// Remove Page
    ///
    /// Removes a page from the extended page table. Note that this function
    /// cleans up as it goes, removing empty extended page tables if they are
    /// detected. For this reason, this operation can be expensive if
    /// mapping / unmapping occurs side by side with addresses that are similar
    /// (extended page tables will be needlessly removed)
    ///
    /// @expects none
    /// @ensures none
    ///
    /// @param addr the virtual address of the page to remove
    ///
    void remove_page(integer_pointer addr)
    { remove_page(addr, intel_x64::ept::pml4::from); }

    /// Find Extended Page Table Entry
    ///
    /// Locates an EPTE given a previously added address.
    ///
    /// @expects none
    /// @ensures none
    ///
    /// @param addr the virtual address of the page to lookup
    ///
    gsl::not_null<ept_entry_intel_x64 *> find_epte(integer_pointer addr)
    { return find_epte(addr, intel_x64::ept::pml4::from); }

private:

    template<class T> std::unique_ptr<T> add_epte(pointer p);
    template<class T> std::unique_ptr<T> remove_epte();

    gsl::not_null<ept_entry_intel_x64 *> add_page(
        integer_pointer addr, integer_pointer bits, integer_pointer end_bits);
    void remove_page(
        integer_pointer addr, integer_pointer bits);
    gsl::not_null<ept_entry_intel_x64 *> find_epte(
        integer_pointer addr, integer_pointer bits);

    auto empty() const noexcept
    { return m_size == 0; }

private:

    gsl::span<integer_pointer> m_ept;
    std::unique_ptr<integer_pointer[]> m_ept_owner;

    size_type m_size;
    integer_pointer m_bitbucket;
    std::vector<std::unique_ptr<ept_entry_intel_x64>> m_eptes;

public:

    ept_intel_x64(ept_intel_x64 &&) noexcept = default;
    ept_intel_x64 &operator=(ept_intel_x64 &&) noexcept = default;

    ept_intel_x64(const ept_intel_x64 &) = delete;
    ept_intel_x64 &operator=(const ept_intel_x64 &) = delete;
};

#endif

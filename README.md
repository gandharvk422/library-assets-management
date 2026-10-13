# Library Assets Management System

A modular C++17 console application for managing a library's physical assets, including books and equipment. Built to demonstrate practical software engineering fundamentals without requiring CI/CD, cloud services, or external dependencies.

## Features

- **Asset catalogue:** Add and list books and equipment with unique IDs, category, creator/manufacturer, location, and quantity.
- **Search:** Search across title, creator/manufacturer, category, asset type, and location.
- **Checkout and return:** Track borrowers, checkout dates, due dates, and returns using loan IDs.
- **Availability control:** Prevents checkout when no copies are available and updates counts on returns.
- **Overdue reporting:** Identify active loans whose due dates are before a date entered by the operator.
- **Dashboard:** View catalogue counts, inventory quantities, active loans, and overdue loans.
- **Persistence:** Saves records locally, so the catalogue remains available after restarting the app.
- **Input and data validation:** Checks required fields, quantities, dates, IDs, and malformed saved records.
- **Modular design:** Separates asset models, library business logic, and the command-line interface.

## Tech stack

- C++17
- CMake
- Standard Library (`vector`, `string`, `algorithm`, `filesystem`, file streams)
- No third-party packages required

## Project structure

```text
library-assets-management/
├── src/
│   ├── Asset.h
│   ├── Asset.cpp
│   ├── Library.h
│   ├── Library.cpp
│   └── main.cpp
├── data/
│   └── README.md
├── .gitignore
├── CMakeLists.txt
└── README.md
```

## Requirements

- A C++17-compatible compiler (GCC 9+, Clang 10+, or a recent MSVC)
- CMake 3.16+

## Build and run

### Windows (Visual Studio Build Tools or another CMake-compatible C++ toolchain)

Open a terminal in the repository root:

```powershell
cmake -S . -B build
cmake --build build --config Release
.\build\Release\library_assets.exe
```

If your generator puts the executable directly in `build`, run:

```powershell
.\build\library_assets.exe
```

### Linux

```bash
cmake -S . -B build
cmake --build build -j
./build/library_assets
```

### Compile directly (without CMake)

If `g++` is installed:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic src/main.cpp src/Asset.cpp src/Library.cpp -o library_assets
./library_assets
```

On Windows, run `.\library_assets.exe` after compiling.

## Quick demo

1. Select **4 — Add asset**.
2. Choose `1` for a book or `2` for equipment.
3. Enter a title, creator/manufacturer, category, location, and quantity.
4. Select **5 — Check out asset**, use the generated asset ID, and enter a borrower and dates in `YYYY-MM-DD` format.
5. Select **7 — Loan register** to see the loan ID.
6. Select **6 — Return asset** and enter that loan ID.
7. Exit and restart the application to confirm that data persists.

## Data storage

Runtime data is stored in `data/assets.db` and `data/loans.db`, created automatically. The app writes temporary files before replacing the saved files. Dates must use ISO format (`YYYY-MM-DD`) so lexical date comparisons work correctly.

The data format is intended for a local demonstration, not concurrent or multi-user deployments. The application is a console program; GitHub hosts the source code and does not run the program as a web service.

## Design overview

- `Asset`: represents a catalogue item and its inventory quantities.
- `Loan`: represents a checkout transaction and its return state.
- `Library`: owns records, applies business rules, and loads/saves local data.
- `main.cpp`: provides a menu-driven interface and presents reports.

## Roadmap

Possible next iterations:
- [ ] Add unit tests for inventory and loan rules
- [ ] Replace flat files with SQLite
- [ ] Add role-based librarian/member accounts
- [ ] Add fine calculation and reservation queues
- [ ] Add a GUI or REST API
- [ ] Add CSV export for reports

## Limitations

- Single-user, local-file application; no network access or authentication.
- Date values are entered by the operator rather than automatically detected.
- The simple data format is not designed for concurrent writes or sensitive personal information.

## License

Released under the MIT License. See `LICENSE`.

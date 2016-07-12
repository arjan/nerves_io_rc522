# nerves_io_rc522

Nerves support for the RC522 RFID reader.


## Installation

If [available in Hex](https://hex.pm/docs/publish), the package can be installed as:

  1. Add `nerves_io_rc522` to your list of dependencies in `mix.exs`:

    ```elixir
    def deps do
      [{:nerves_io_rc522, "~> 0.1.0"}]
    end
    ```

  2. Ensure `nerves_io_rc522` is started before your application:

    ```elixir
    def application do
      [applications: [:nerves_io_rc522]]
    end
    ```

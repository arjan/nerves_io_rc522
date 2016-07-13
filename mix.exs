defmodule NervesIoRc522.Mixfile do
  use Mix.Project

  def project do
    [app: :nerves_io_rc522,
     version: "0.1.0",
     elixir: "~> 1.2",
     name: "nerves_io_rc522",
     description: description(),
     package: package(),
     source_url: "https://github.com/arjan/nerves_io_rc522",
     compilers: [:elixir_make] ++ Mix.compilers,
     make_clean: ["clean"],
     build_embedded: Mix.env == :prod,
     start_permanent: Mix.env == :prod,
     deps: deps()]
  end

  def application do
    [applications: [:logger]]
  end

  defp description do
  """
  Elixir access to the RC522 RFID reader module over SPI
  """
  end

  defp package do
    %{files: ["lib", "src/*.[ch]", "mix.exs", "README.md", "LICENSE", "Makefile"],
      maintainers: ["Arjan Scherpenisse"],
      licenses: ["Apache-2.0"],
      links: %{"GitHub" => "https://github.com/arjan/nerves_io_rc522"}}
  end

  defp deps do
    [
      {:elixir_make, "~> 0.3"}
    ]
  end
end

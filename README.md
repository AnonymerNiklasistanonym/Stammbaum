# Stammbaum

Program with which one can document the common family tree in collaboration with other family members

## Known issues

- The `clang` compiler is currently not supported because the vendor dependency [`sqlpp11`](vendor/sqlpp11) throws errors

   ```text
   server/src/main.cpp:195:47: error: use of overloaded operator '!=' is ambiguous (with operand types 'const sqlpp::column_t<TabSample, TabSample_::Gamma>' and 'parameter_t<value_type_of<sqlpp::column_t<TabSample, TabSample_::Gamma> >, sqlpp::column_t<TabSample, TabSample_::Gamma> >' (aka 'parameter_t<sqlpp::boolean, sqlpp::column_t<TabSample, TabSample_::Gamma> >'))
                                    tab.gamma != sqlpp::parameter(tab.gamma)));
   ```

TODO
====

 - ComponentType by mohol by mapovany na u32 a tym padom by nemusel
   byt registrovany v meta (Type::ComponentType ani v set_value)
 - delete Vec2Array... or replace by Slice
 - allow Entity without ModelComponent, just MeshComponent
 - proper clone method, Component attributes are not copied
 - rename VideoBuffer::set_raw_data to set_bytes???
 - premenovat MAP_COMPONENT_TYPE na napr. DECLARE_COMPONENT, DEFINE_COMPONENT,
   "INSTALL_COMPONENT", "REGISTER_COMPONENT"
 - debug per entity, per component?
 - zmazat Singleton
 - rozlisenie medzi uniform a uniform array sa deje na zaklade parametru size,
   ktory je pre polia vacsi ako 1, vynimkou je pole velkosti 1, riesenim je asi
   rozlisovat medzi scalar a array podla nazvu pretoze pole ma nazov + [0]

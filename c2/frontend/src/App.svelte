<script>
  import { Router, Route, Link } from "svelte-routing";
  import Home from "./Home.svelte";
  import Login from "./Login.svelte";
  import Authenticated from "./Authenticated.svelte";
  import Implant from "./Implant.svelte";

  export const routes = [
  Route
  ]
</script>

<Router>
  <nav>
    <!-- <Link to="/">Home</Link> -->
    <Link to="/login">Login</Link>
    <Link to="/authenticated">Authenticated</Link>
  </nav>

  <!-- <Route path="/" component={Home} /> -->
  <Route path="/login" component={Login} />
  <Route path="/authenticated" component={Authenticated} />
  <Route path="/implant" component={Implant} />
</Router>

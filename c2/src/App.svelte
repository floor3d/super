<script>
  import { Router, Route, Link } from "svelte-routing";
  import Home from "./routes/Home.svelte";
  import Login from "./routes/Login.svelte";
  import Authenticated from "./routes/Authenticated.svelte";
</script>

<Router>
  <nav>
    <Link to="/">Home</Link>
    <Link to="/login">Login</Link>
    <Link to="/authenticated">Authenticated</Link>
  </nav>
  <Route path="/" component={Home} />
  <Route path="login" component={Login} />
  <Route path="authenticated" component={Authenticated} />
</Router>
